#include "grid_fluid_simulator.h"
#include <QThread>
#include "spdlog/spdlog.h"
#include <cmath>
#include <iostream>

const float FLOW_RATE = 0.1f;

GridFluidSimulator::GridFluidSimulator(uint32_t width,      //
                                       uint32_t height,     //
                                       float delta_t,       //
                                       float diffusion_rate //
                                       )                    //
    : FluidSimulator2D{width, height}                       //
    , delta_t_{delta_t}                                     //
    , diffusion_rate_{diffusion_rate}                       //
{
    Initialise();
}

void GridFluidSimulator::InitialiseDensity()
{
    // Initialise with a blob in the middle
    auto rad = dim_x_ / 4.0f;
    auto rad2 = rad * rad;
    float cx = dim_x_ / 2.0f;
    float cy = dim_y_ / 2.0f;
    for (int y = 0; y < dim_y_; y++) {
        for (int x = 0; x < dim_x_; x++) {
            float dx = x + 0.5f - cx;
            float dy = y + 0.5f - cy;
            float d2 = dx * dx + dy * dy;
            density_.at(Index(x, y)) = (d2 < rad2) ? 1.0f : 0.0f;
        }
    }
}

void GridFluidSimulator::InitialiseVelocity()
{
    // Initialise a spiral
    /*
     * fx   |   fy   |   vx  |  vy
     * -----+--------+-------+-----
     *   0  |   .01  |  0.0  | 0.5
     *   0  |  -.01  |  0.0  |-0.5
     *   0  |   .5   |  0.0  | 0.0
     *   0  |  -.5   |  0.0  |-0.0
     */
    for (int y = 0; y < dim_y_; y++) {
	for (int x = 0; x < dim_x_; x++) {
	    auto fx = (float) x / dim_x_;
	    auto fy = (float) y / dim_y_;
	    velocity_x_.at(Index(x, y)) = fy - fx;
	    velocity_y_.at(Index(x, y)) = fx - fy;
	}
    }
}

void GridFluidSimulator::Diffuse(const std::vector<float>& current_density,
				 std::vector<float>& next_density)
{
    const uint32_t NUM_GS_ITERS = 5;
    // Initialise target_density with current values because why not
    std::memcpy(next_density.data(), current_density.data(), num_cells_ * sizeof(float));

    // Run four iterations of GS
    // Dn(x,y) = Dc(x,y) + (k*0.25*(Dn(x+1,y)+Dn(x-1,y)+Dn(x,y+1)+Dn(x,y-1)))/(1+k)

    auto k = delta_t_ * diffusion_rate_;
    for (auto iter = 0; iter < NUM_GS_ITERS; ++iter) {
	for (auto y = 1; y < dim_y_ - 1; ++y) {
	    for (auto x = 1; x < dim_x_ - 1; ++x) {
	      auto idx = Index(x, y);

	      float total_nbrs = 0.0f;
	      float nbr_count = 0.0f;
	      if (x > 1) {
		total_nbrs += next_density.at(idx - 1);
		nbr_count += 1.0f;
	      }
	      if (x < dim_x_ - 1) {
		total_nbrs += next_density.at(idx + 1);
		nbr_count += 1.0f;
	      }
	      if (y > 0) {
		total_nbrs += next_density.at(idx - dim_x_);
		nbr_count += 1.0f;
	      }
	      if (y < dim_y_ - 1) {
		total_nbrs += next_density.at(idx + dim_x_);
		nbr_count += 1.0f;
	      }
	      auto mean_nbr = total_nbrs / nbr_count;

	      //x[IX(i,j)] = (x0[IX(i,j)] + a*(x[IX(i-1,j)]+x[IX(i+1,j)]+x[IX(i,j-1)]+x[IX(i,j+1)]))/(1+4*a)

	      auto new_val = (current_density.at(idx) + (k * mean_nbr)) / (k + 1.0f);
	      next_density.at(idx) = new_val;
	    }
	}
    }

    auto start_sum = 0.0f;
    auto start_bdry = 0.0f;
    auto end_sum = 0.0f;
    auto end_bdry = 0.0f;
    for (auto y = 0; y < dim_y_; ++y) {
	for (auto x = 0; x < dim_x_; ++x) {
	    if (x == 0 || y == 0 || x == dim_x_ - 1 || y == dim_y_ - 1) {
	      start_bdry += current_density.at(Index(x, y));
	      end_bdry += next_density.at(Index(x, y));
	    } else {
	      start_sum += current_density.at(Index(x, y));
	      end_sum += next_density.at(Index(x, y));
	    }
	}
    }
    spdlog::debug("Diffusion S: {:0.3f}  SB: {:0.3f} E: {:0.3f}  EB: {:0.3f}  L: {:0.3f}",
		  start_sum,
		  start_bdry,
		  end_sum,
		  end_bdry,
		  end_sum - start_sum);
}

float GridFluidSimulator::AdvectValue(const std::vector<float>& source_data,
                                      uint32_t x,
                                      uint32_t y) const
{
    auto idx = Index(x, y);

    // Get the velocity for this cell
    auto vx = velocity_x_.at(idx);
    auto vy = velocity_y_.at(idx);

    // Get the source point for that flow
    auto source_x = (x) -vx * delta_t_;
    auto source_y = (y) -vy * delta_t_;
    source_x = std::fmaxf(0.5f, std::fminf(dim_x_ - 0.5f, source_x));
    source_y = std::fmaxf(0.5f, std::fminf(dim_y_ - 0.5f, source_y));

    // Get the base coord
    auto base_x = (uint32_t) std::floorf(source_x);
    auto base_y = (uint32_t) std::floorf(source_y);

    // And the fractional offset
    auto frac_x = source_x - base_x;
    auto frac_y = source_y - base_y;

    // Interpolate top and bottom
    auto top_lerp = Lerp(source_data.at(Index(base_x, base_y)),
			 source_data.at(Index(base_x + 1, base_y)),
			 frac_x);
    auto btm_lerp = Lerp(source_data.at(Index(base_x, base_y + 1)),
			 source_data.at(Index(base_x + 1, base_y + 1)),
			 frac_x);
    auto final_val = Lerp(top_lerp, btm_lerp, frac_y);
    // interpolate top to bottom
    return final_val;
}

void GridFluidSimulator::AdvectDensity(const std::vector<float>& curr_density,
				       std::vector<float>& advected_density) const
{
    std::fill(advected_density.begin(), advected_density.end(), 0.0f);
    for (auto y = 1; y < dim_y_ - 1; ++y) {
	for (auto x = 1; x < dim_x_ - 1; ++x) {
	    advected_density.at(Index(x, y)) = AdvectValue(curr_density, x, y);
	}
    }
}

void GridFluidSimulator::AdvectVelocity()
{
    std::vector<float> advected_velocity_x(num_cells_);
    std::vector<float> advected_velocity_y(num_cells_);

    for (auto y = 1; y < dim_y_ - 1; ++y) {
        for (auto x = 1; x < dim_x_ - 1; ++x) {
            auto idx = Index(x, y);
            advected_velocity_x.at(idx) = AdvectValue(velocity_x_, x, y);
            advected_velocity_y.at(idx) = AdvectValue(velocity_y_, x, y);
        }
    }
    std::memcpy(velocity_x_.data(), advected_velocity_x.data(), num_cells_ * sizeof(float));
    std::memcpy(velocity_y_.data(), advected_velocity_y.data(), num_cells_ * sizeof(float));
}

/*
 * d(x,y) = [ vx(x+1,y) - vx(x-1,y) + vy(x,y+1)-vy(x,y-1) ] * 0.5f
 */
void GridFluidSimulator::ComputeDivergence(std::vector<float>& divergence) const
{
    for (auto y = 1; y < dim_y_ - 1; ++y) {
	for (auto x = 1; x < dim_x_ - 1; ++x) {
	    auto idx = Index(x, y);
	    divergence.at(idx) =                   //
		(                                  //
		    velocity_x_.at(idx + 1) -      //
		    velocity_x_.at(idx - 1) +      //
		    velocity_y_.at(idx + dim_x_) - //
		    velocity_y_.at(idx - dim_x_)   //
		    )
		* 0.5f;
	}
    }
}

/*
 * Compute pressure and solve to obtain stable field
 * 0.25f * [p(x-1,y)+p(x+1,y)+p(x,y-1)+p(x,y+1)- divergence(x,y)] =p(x,y)
 */
void GridFluidSimulator::ComputePressure(const std::vector<float>& divergence,
					 std::vector<float>& pressure) const
{
    // Set pressure to zero everywhere
    std::fill(pressure.begin(), pressure.end(), 0);
    std::vector<float> temp_pressure(num_cells_, 0);
    std::fill(temp_pressure.begin(), temp_pressure.end(), 0);

    // TODO: Parameterise this count
    for (auto iter = 0; iter < 4; ++iter) {
	for (auto y = 1; y < dim_y_ - 1; ++y) {
	    for (auto x = 1; x < dim_x_ - 1; ++x) {
		auto idx = Index(x, y);

		auto p = (                               //
			     pressure.at(idx - 1) +      //
			     pressure.at(idx + 1) +      //
			     pressure.at(idx - dim_x_) + //
			     pressure.at(idx + dim_x_) - //
			     divergence.at(idx)          //
			     )
			 * 0.25f;
		temp_pressure.at(idx) = p;
	    }
	}
	std::memcpy(pressure.data(), temp_pressure.data(), num_cells_ * sizeof(float));
    }
}

/*
 * \nabla p(x,y) =0.5f * [  p(x+1,y) - p(x-1),y), p(x,y+1)-p(x,y-1) ]
 */
void GridFluidSimulator::ComputeCurlField(const std::vector<float>& pressure,
					  std::vector<float>& curl_x,
					  std::vector<float>& curl_y) const
{
    for (auto y = 1; y < dim_y_ - 1; ++y) {
	for (auto x = 1; x < dim_x_ - 1; ++x) {
	    auto idx = Index(x, y);
	    curl_x.at(idx) = (pressure.at(idx + 1) - pressure.at(idx - 1)) * 0.5f;
	    curl_y.at(idx) = (pressure.at(idx + dim_x_) - pressure.at(idx - dim_x_)) * 0.5f;
	}
    }
}
void GridFluidSimulator::SuppressDivergence()
{
    std::vector<float> divergence(num_cells_, 0);
    ComputeDivergence(divergence);
    std::vector<float> pressure(num_cells_, 0);
    ComputePressure(divergence, pressure);
    std::vector<float> curl_x(num_cells_, 0);
    std::vector<float> curl_y(num_cells_, 0);
    ComputeCurlField(pressure, curl_x, curl_y);

    for (auto i = 0; i < num_cells_; ++i) {
	velocity_x_.at(i) -= curl_x.at(i);
	velocity_y_.at(i) -= curl_y.at(i);
    }
}

void GridFluidSimulator::CorrectBoundaryDensities()
{
    // Horizontal boundaries
    for (auto x = 1; x < dim_x_ - 1; ++x) {
	// Top
	density_.at(Index(x, 0)) = density_.at(Index(x, 1));
	density_.at(Index(x, dim_y_ - 1)) = density_.at(Index(x, dim_y_ - 2));
    }
    // Vertical boundaries
    for (auto y = 1; y < dim_y_ - 1; ++y) {
	density_.at(Index(0, y)) = density_.at(Index(1, y));
	density_.at(Index(dim_x_ - 1, y)) = density_.at(Index(dim_x_ - 2, y));
    }
}

void GridFluidSimulator::CorrectBoundaryVelocities()
{
    // Horizontal boundaries
    for (auto x = 1; x < dim_x_ - 1; ++x) {
	// Top
	velocity_x_.at(Index(x, 0)) = velocity_x_.at(Index(x, 1));
	velocity_y_.at(Index(x, 0)) = -velocity_y_.at(Index(x, 1));

	// Bottom
	velocity_x_.at(Index(x, dim_y_ - 1)) = velocity_x_.at(Index(x, dim_y_ - 2));
	velocity_y_.at(Index(x, dim_y_ - 1)) = -velocity_y_.at(Index(x, dim_y_ - 2));
    }
    // Vertical boundaries
    for (auto y = 1; y < dim_y_ - 1; ++y) {
	// Left
	velocity_x_.at(Index(0, y)) = -velocity_x_.at(Index(1, y));
	velocity_y_.at(Index(0, y)) = velocity_y_.at(Index(1, y));
	// Right
	velocity_x_.at(Index(dim_x_ - 1, y)) = -velocity_x_.at(Index(dim_x_ - 2, y));
	velocity_y_.at(Index(dim_x_ - 1, y)) = velocity_y_.at(Index(dim_x_ - 2, y));
    }
    // Corners are not accessible during calcs so we can ignore.
}

void GridFluidSimulator::Simulate()
{
    std::vector<float> target_density(num_cells_);
    CorrectBoundaryDensities();
    Diffuse(density_, target_density);
    std::memcpy(density_.data(), target_density.data(), num_cells_ * sizeof(float));

    CorrectBoundaryDensities();
    CorrectBoundaryVelocities();
    AdvectDensity(density_, target_density);
    std::memcpy(density_.data(), target_density.data(), num_cells_ * sizeof(float));

    AdvectVelocity();
    CorrectBoundaryVelocities();
    SuppressDivergence();
}

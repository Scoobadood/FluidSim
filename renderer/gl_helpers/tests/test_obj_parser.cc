#include "gtest/gtest.h"
#include "mesh_internal.h"

#include <fstream>

class TestObjLoader : public ::testing::Test {
};

TEST_F(TestObjLoader, parse_3f_succeeds_with_warning) {
  std::string txt = "0.1 0.2 0.3 0.4";
  float x, y, z;
  auto actual = parse_3f(txt, x, y, z);
  EXPECT_TRUE(actual);
  EXPECT_EQ(0.1f, x);
  EXPECT_EQ(0.2f, y);
  EXPECT_EQ(0.3f, z);
}

TEST_F(TestObjLoader, parse_3f_fails_with_too_few) {
  std::string txt = "0.1 0.2";
  float x, y, z;
  auto actual = parse_3f(txt, x, y, z);
  EXPECT_FALSE(actual);
}

TEST_F(TestObjLoader, parse_3f_handles_extraneous_spacing) {
  std::string txt = "  0.1     0.2   0.5";
  float x, y, z;
  auto actual = parse_3f(txt, x, y, z);
  EXPECT_TRUE(actual);
  EXPECT_EQ(0.1f, x);
  EXPECT_EQ(0.2f, y);
  EXPECT_EQ(0.5f, z);
}

TEST_F(TestObjLoader, parse_3f_handles_empty_string) {
  std::string txt;
  float x, y, z;
  auto actual = parse_3f(txt, x, y, z);
  EXPECT_FALSE(actual);
}


TEST_F(TestObjLoader, parse_2f_succeeds_with_too_many_with_warning) {
  std::string txt = "0.1 0.2 0.4";
  float x, y;
  auto actual = parse_2f(txt, x, y);
  EXPECT_TRUE(actual);
  EXPECT_EQ(0.1f, x);
  EXPECT_EQ(0.2f, y);
}

TEST_F(TestObjLoader, parse_2f_fails_with_too_few) {
  std::string txt = "0.1 ";
  float x, y;
  auto actual = parse_2f(txt, x, y);
  EXPECT_FALSE(actual);
}

TEST_F(TestObjLoader, parse_2f_handles_extraneous_spacing) {
  std::string txt = "  0.1    0.5";
  float x, y;
  auto actual = parse_2f(txt, x, y);
  EXPECT_TRUE(actual);
  EXPECT_EQ(0.1f, x);
  EXPECT_EQ(0.5f, y);
}

TEST_F(TestObjLoader, parse_2f_handles_empty_string) {
  std::string txt;
  float x, y;
  auto actual = parse_2f(txt, x, y);
  EXPECT_FALSE(actual);
}


TEST_F(TestObjLoader, parse_face_rejects_inconsistent_faces) {
  std::string txt = "1 2/2 3/3/3";
  int32_t v[3], n[3], t[3];
  auto actual = parse_face(txt, v, true, n, true, t);
  EXPECT_FALSE(actual);
}


TEST_F(TestObjLoader, parse_face_elements_rejects_leading_slash) {
  std::string txt = "/1/2/3";
  int32_t v, t, n;
  auto actual = parse_face_elements(txt, &v,
                                    true, &n,
                                    true, &t);
  EXPECT_FALSE(actual);
}

TEST_F(TestObjLoader, parse_face_elements_rejects_trailing_slash) {
  std::string txt = "1/2/3/";
  int32_t v, t, n;
  auto actual = parse_face_elements(txt, &v,
                                    true, &n,
                                    true, &t);
  EXPECT_FALSE(actual);
}

TEST_F(TestObjLoader, parse_face_elements_rejects_embedded_spaces) {
  std::string txt = "1 /2/3";
  int32_t v, t, n;
  auto actual = parse_face_elements(txt, &v,
                                    true, &n,
                                    true, &t);
  EXPECT_FALSE(actual);
}

TEST_F(TestObjLoader, parse_face_elements_trims_leading_spaces) {
  std::string txt = " 1/2/3";
  int32_t v, t, n;
  auto actual = parse_face_elements(txt, &v,
                                    true, &n,
                                    true, &t);
  EXPECT_TRUE(actual);
  EXPECT_EQ(1, v);
  EXPECT_EQ(2, t);
  EXPECT_EQ(3, n);
}

TEST_F(TestObjLoader, parse_face_elements_trims_trailing_spaces) {
  std::string txt = "1/2/3 ";
  int32_t v, t, n;
  auto actual = parse_face_elements(txt, &v,
                                    true, &n,
                                    true, &t);
  EXPECT_TRUE(actual);
  EXPECT_EQ(1, v);
  EXPECT_EQ(2, t);
  EXPECT_EQ(3, n);
}

TEST_F(TestObjLoader, pfe_rejects_request_for_missing_tex_1) {
  std::string txt = "1";
  int32_t v, t, n;
  auto actual = parse_face_elements(txt, &v,
                                    false, &n,
                                    true, &t);
  EXPECT_FALSE(actual);
}

TEST_F(TestObjLoader, pfe_rejects_request_for_missing_tex_2) {
  std::string txt = "1//2";
  int32_t v, t, n;
  auto actual = parse_face_elements(txt, &v,
                                    false, &n,
                                    true, &t);
  EXPECT_FALSE(actual);
}

TEST_F(TestObjLoader, pfe_rejects_request_for_missing_norm_1) {
  std::string txt = "1";
  int32_t v, t, n;
  auto actual = parse_face_elements(txt, &v,
                                    true, &n,
                                    false, &t);
  EXPECT_FALSE(actual);
}

TEST_F(TestObjLoader, pfe_rejects_request_for_missing_norm_2) {
  std::string txt = "1/2";
  int32_t v, t, n;
  auto actual = parse_face_elements(txt, &v,
                                    true, &n,
                                    false, &t);
  EXPECT_FALSE(actual);
}

TEST_F(TestObjLoader, pfe_retrieves_vertex_norm_with_no_tex) {
  std::string txt = "1//3";
  int32_t v, t, n;
  auto actual = parse_face_elements(txt, &v,
                                    true, &n,
                                    false, &t);
  EXPECT_TRUE(actual);
  EXPECT_EQ(1, v);
  EXPECT_EQ(3, n);
}

TEST_F(TestObjLoader, pfe_retrieves_vertex_norm_with_tex) {
  std::string txt = "1/2/3";
  int32_t v, t, n;
  auto actual = parse_face_elements(txt, &v,
                                    true, &n,
                                    false, &t);
  EXPECT_TRUE(actual);
  EXPECT_EQ(1, v);
  EXPECT_EQ(3, n);
}

TEST_F(TestObjLoader, pfe_retrieves_vertex_tex_with_no_norm) {
  std::string txt = "1/2";
  int32_t v, t, n;
  auto actual = parse_face_elements(txt, &v,
                                    false, &n,
                                    true, &t);
  EXPECT_TRUE(actual);
  EXPECT_EQ(1, v);
  EXPECT_EQ(2, t);
}

TEST_F(TestObjLoader, pfe_retrieves_vertex_tex_with_norm) {
  std::string txt = "1/2/3";
  int32_t v, t, n;
  auto actual = parse_face_elements(txt, &v,
                                    false, &n,
                                    true, &t);
  EXPECT_TRUE(actual);
  EXPECT_EQ(1, v);
  EXPECT_EQ(2, t);
}


TEST_F(TestObjLoader, parse_real_file_ok) {
  using namespace std;

  vector<tuple<float, float, float>> vertices;
  vector<vector<tuple<int32_t, int32_t, int32_t>>> faces;
  vector<tuple<float, float, float>> normals;
  vector<tuple<float, float>> tex_coords;

  ifstream in("/Users/dave/CLionProjects/utah_icg/data/african_head.obj");
  auto ok = parse_raw_data(in, vertices, faces, true, &normals, true, &tex_coords);
  EXPECT_TRUE(ok);
}


int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

#include <array>
#include <cassert>
#include <iostream>
#include <string>

#include "ModelWrapper.h"

int main(int, char**) {

  std::string const libA{"TestModelA"};
  std::string const libB{"TestModelB"};

  hls4mlEmulator::ModelWrapper mw1{libA};
  hls4mlEmulator::ModelWrapper mw2{libA};
  hls4mlEmulator::ModelWrapper mw3{libB};

  std::array<int, 3> mw1_inputs{1, 2, 3};
  std::array<int, 3> mw2_inputs{4, 5, 6};
  std::array<int, 3> mw3_inputs{7, 8, 9};

  int mw1_output{0};
  int mw2_output{0};
  int mw3_output{0};

  int const expected_output_libA_mw1inputs{5};
  int const expected_output_libA_mw2inputs{26};
  int const expected_output_libB_mw1inputs{7};
  int const expected_output_libB_mw2inputs{34};
  int const expected_output_libB_mw3inputs{79};

  // Create copies of two wrappers, run inference and
  // destroy them before the other wrappers are used
  {
    hls4mlEmulator::ModelWrapper mw1_copy{mw1.model_name()};
    hls4mlEmulator::ModelWrapper mw3_copy{mw3.model_name()};

    mw1.run_inference(mw1_inputs, &mw1_output);
    assert(mw1_output == expected_output_libA_mw1inputs);

    mw3.run_inference(mw3_inputs, &mw3_output);
    assert(mw3_output == expected_output_libB_mw3inputs);
  }

  // Run inference on two instances of the same Model, plus a second Model
  mw1.run_inference(mw1_inputs, &mw1_output);
  assert(mw1_output == expected_output_libA_mw1inputs);

  mw2.run_inference(mw2_inputs, &mw2_output);
  assert(mw2_output == expected_output_libA_mw2inputs);

  mw3.run_inference(mw3_inputs, &mw3_output);
  assert(mw3_output == expected_output_libB_mw3inputs);

  // Reset a model wrapper
  mw1.reset();

  // Change model, and rerun inference
  mw2.reset(libB);
  mw2.run_inference(mw2_inputs, &mw2_output);
  assert(mw2_output == expected_output_libB_mw2inputs);

  // Update model via move operator, and rerun inference
  mw1 = std::move(mw3);
  mw1.run_inference(mw1_inputs, &mw1_output);
  assert(mw1_output == expected_output_libB_mw1inputs);

  std::cout << "------------------------------" << std::endl;
  std::cout << "testModelWrapper: tests passed" << std::endl;
  std::cout << "------------------------------" << std::endl;

  return 0;
}

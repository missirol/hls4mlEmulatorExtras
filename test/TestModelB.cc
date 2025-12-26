#include <any>
#include <array>

#include "Model.h"

class TestModelB : public hls4mlEmulator::Model {
private:
  using inputs_t = std::array<int, 3>;
  using output_t = int;
  inputs_t inputs_;
  output_t output_;

public:
  TestModelB() : inputs_{0, 0, 0}, output_{0} {}

  virtual ~TestModelB() = default;

  virtual void prepare_input(std::any input) { inputs_ = std::any_cast<inputs_t>(input); }

  virtual void predict() { output_ = inputs_[0] + inputs_[1] * inputs_[2]; }

  virtual void read_result(std::any result) {
    output_t* output = std::any_cast<output_t*>(result);
    *output = output_;
  }
};

extern "C" hls4mlEmulator::Model* create_model() { return new TestModelB; }

extern "C" void destroy_model(hls4mlEmulator::Model* m) { delete m; }

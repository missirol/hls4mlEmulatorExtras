#ifndef hls4mlEmulator_Model_h
#define hls4mlEmulator_Model_h

#include <any>

namespace hls4mlEmulator {

  class Model {
  public:
    virtual void prepare_input(std::any input) = 0;
    virtual void predict() = 0;
    virtual void read_result(std::any result) = 0;
    virtual ~Model() = default;
  };

}

#endif

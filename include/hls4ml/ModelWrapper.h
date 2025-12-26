#ifndef hls4mlEmulator_ModelWrapper_h
#define hls4mlEmulator_ModelWrapper_h

#include <any>
#include <functional>
#include <memory>
#include <string>

#include "Model.h"

namespace hls4mlEmulator {

  class ModelWrapper {
  public:
    ModelWrapper();
    ModelWrapper(std::string const& model_name);
    ~ModelWrapper() = default;

    ModelWrapper(ModelWrapper const&) = delete;
    ModelWrapper& operator=(ModelWrapper const&) = delete;
    ModelWrapper(ModelWrapper&&) noexcept = default;
    ModelWrapper& operator=(ModelWrapper&&) noexcept = default;

    void reset();
    void reset(std::string const& model_name);

    void run_inference(std::any input, std::any result) const;

    std::string const& model_name() const { return model_name_; }

    bool model_loaded() const { return (model_ != nullptr); }

  private:
    using create_model_cls = Model*();
    using destroy_model_cls = void(Model*);
    using model_ptr = std::unique_ptr<Model, std::function<destroy_model_cls>>;

    void load();

    model_ptr model_;
    std::string model_name_;
  };

}

#endif

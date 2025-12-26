#include <dlfcn.h>
#include <stdexcept>

#include "ModelWrapper.h"

hls4mlEmulator::ModelWrapper::ModelWrapper() : model_{nullptr}, model_name_{""} {}

hls4mlEmulator::ModelWrapper::ModelWrapper(std::string const& model_name) : model_{nullptr}, model_name_{model_name} {
  load();
}

void hls4mlEmulator::ModelWrapper::reset() {
  model_.reset();
  model_name_ = "";
}

void hls4mlEmulator::ModelWrapper::reset(std::string const& model_name) {
  model_.reset();
  model_name_ = model_name;
  load();
}

void hls4mlEmulator::ModelWrapper::run_inference(std::any input, std::any result) const {
  if (not model_loaded()) {
    throw std::runtime_error("failed call to run_inference: hls4ml model not loaded yet !");
  }

  model_->prepare_input(input);
  model_->predict();
  model_->read_result(result);
}

void hls4mlEmulator::ModelWrapper::load() {
  if (model_loaded()) {
    throw std::runtime_error("failed to load hls4ml model: a model is already loaded !");
  }

  // open the shared library containing the implementation of the model
  std::string const model_lib_name = model_name_ + ".so";
  std::shared_ptr<void> model_lib{dlopen(model_lib_name.c_str(), RTLD_LAZY | RTLD_LOCAL), [](void* lib) {
                                    if (lib != nullptr) {
                                      dlclose(lib);
                                    }
                                  }};
  if (model_lib == nullptr) {
    throw std::runtime_error("hls4ml model library dlopen failure: cannot load library \"" + model_lib_name + "\" !");
  }

  // "create_model" function: it creates an instance of the model and returns a pointer to the model
  create_model_cls* create_model = (create_model_cls*)dlsym(model_lib.get(), "create_model");
  if (dlerror()) {
    throw std::runtime_error("hls4ml emulator failed to load 'create_model' symbol from library \"" + model_lib_name + "\" !");
  }

  // "destroy_model" function: deletes the model
  destroy_model_cls* destroy_model = (destroy_model_cls*)dlsym(model_lib.get(), "destroy_model");
  if (dlerror()) {
    throw std::runtime_error("hls4ml emulator failed to load 'destroy_model' symbol from library \"" + model_lib_name + "\" !");
  }

  // smart pointer to the model with its own custom deleter;
  // the custom deleter holds a smart pointer to the model's shared library,
  // so that the shared library will be closed right after the model is deleted
  model_ = model_ptr(create_model(), [model_lib, destroy_model](Model* m) { destroy_model(m); });
  if (model_ == nullptr) {
    throw std::runtime_error("hls4ml emulator failed to load model (nullptr) from library \"" + model_lib_name + "\" !");
  }
}

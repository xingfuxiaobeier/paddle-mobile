/* Copyright (c) 2018 PaddlePaddle Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */
#ifdef FUSION_ELEMENTWISEADDRELU_OP

#include "operators/kernel/elementwise_add_relu_kernel.h"

namespace paddle_mobile {
namespace operators {

template <>
bool ElementwiseAddReluKernel<FPGA, float>::Init(
    ElementwiseAddReluParam<FPGA> *param) {
  bool relu_enabled = true;
  auto *input_x = const_cast<LoDTensor *>(param->InputX());
  auto *input_y = const_cast<LoDTensor *>(param->InputY());
  auto *out = param->Out();
  auto input_x_ptr = input_x->data<float>();
  auto input_y_ptr = input_y->data<float>();
  fpga::format_fp16_ofm(out);
  auto out_ptr = out->mutable_data<float>();

  fpga::EWAddArgs ewaddArgs = {0};
  ewaddArgs.relu_enabled = relu_enabled;
  ewaddArgs.const0 = 0x3c00;  // =1
  ewaddArgs.const1 = 0x3c00;  // =1
  ewaddArgs.image0.address = input_x_ptr;
  ewaddArgs.image0.channels = (uint32_t)input_x->dims()[1];
  ewaddArgs.image0.scale_address = input_x->scale;
  ewaddArgs.image0.height = (uint32_t)input_x->dims()[2];
  ewaddArgs.image0.width = (uint32_t)input_x->dims()[3];
  ewaddArgs.image0.pad_height = 0;
  ewaddArgs.image0.pad_width = 0;
  ewaddArgs.image1.address = input_y_ptr;
  ewaddArgs.image1.channels = (uint32_t)input_y->dims()[1];
  ewaddArgs.image1.scale_address = input_y->scale;
  ewaddArgs.image1.height = (uint32_t)input_y->dims()[2];
  ewaddArgs.image1.width = (uint32_t)input_y->dims()[3];
  ewaddArgs.image1.pad_height = 0;
  ewaddArgs.image1.pad_width = 0;
  ewaddArgs.output.scale_address = out->scale;
  ewaddArgs.output.address = out_ptr;
  param->SetFpgaArgs(ewaddArgs);
  return true;
}

template <>
void ElementwiseAddReluKernel<FPGA, float>::Compute(
    const ElementwiseAddReluParam<FPGA> &param) const {
  fpga::ComputeFpgaEWAdd(param.FpgaArgs());
}
}  // namespace operators
}  // namespace paddle_mobile

#endif

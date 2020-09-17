// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "gsl/gsl"
#include "core/providers/cuda/cuda_common.h"

namespace onnxruntime {
namespace contrib {
namespace cuda {
  
template <typename T>
struct AccumulateType {};
template <>
struct AccumulateType<float> { using type = float; };
template <>
struct AccumulateType<MLFloat16> { using type = float; };
template <>
struct AccumulateType<double> { using type = double; };
template <typename T>
using AccType = typename AccumulateType<T>::type;

template <typename T> 
struct DispatchBiasSoftmaxForward;

template <typename T>
void DispatchBiasSoftmaxForwardImpl(
    Tensor* output_tensor, 
    const Tensor* input_tensor, 
    const Tensor* input_bias_tensor, 
    int element_count, 
    int batch_count, 
    int batch_stride, 
    int bias_broadcast_size_per_batch);

template <typename T> 
struct DispatchBiasSoftMaxForwardViaDnnLibrary;

template <typename T>
void DispatchBiasSoftMaxForwardViaDnnLibraryImpl(
    cudnnHandle_t cudaDnnHandle,
    int element_count,
    int batch_count,
    int broadcast_axis,
    int softmax_axis,
    const onnxruntime::TensorShape& X_shape,
    const onnxruntime::Tensor* X,
    const onnxruntime::TensorShape& B_shape,
    const onnxruntime::Tensor* B,
    onnxruntime::Tensor* Y);

class BiasSoftmax final : public onnxruntime::cuda::CudaKernel {
 public:
  BiasSoftmax(const OpKernelInfo& info) : CudaKernel{info} {
    info.GetAttrOrDefault("softmax_axis", &softmax_axis_, static_cast<int64_t>(1));
    info.GetAttrOrDefault("broadcast_axis", &broadcast_axis_, static_cast<int64_t>(1));
  }

  Status ComputeInternal(OpKernelContext* context) const override;

 private:
  int64_t softmax_axis_;
  int64_t broadcast_axis_;
};

}  // namespace cuda
}  // namespace contrib
}  // namespace onnxruntime
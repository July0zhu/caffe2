/**
 * Copyright (c) 2016-present, Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CAFFE2_OPERATORS_SPATIAL_BATCH_NORM_OP_H_
#define CAFFE2_OPERATORS_SPATIAL_BATCH_NORM_OP_H_

#include "caffe2/core/context.h"
#include "caffe2/core/operator.h"
#include "caffe2/utils/math.h"

namespace caffe2 {

template <class Context>
class SpatialBNOp : public Operator<Context> {
 public:
  USE_OPERATOR_CONTEXT_FUNCTIONS;
  SpatialBNOp(const OperatorDef& operator_def, Workspace* ws)
      : Operator<Context>(operator_def, ws),
        is_test_(OperatorBase::GetSingleArgument<int>(OpSchema::Arg_IsTest, 0)),
        epsilon_(OperatorBase::GetSingleArgument<float>("epsilon", 1e-5)),
        momentum_(OperatorBase::GetSingleArgument<float>("momentum", 0.9)),
        order_(StringToStorageOrder(
            OperatorBase::GetSingleArgument<string>("order", "NCHW"))) {
    // TODO(jiayq): update the input and output size checks.
    CAFFE_ENFORCE(
        (is_test_ && OutputSize() == 1) || (!is_test_ && OutputSize() == 5));
    CAFFE_ENFORCE_GT(epsilon_, 0);
    CAFFE_ENFORCE_GE(momentum_, 0);
    CAFFE_ENFORCE_LE(momentum_, 1);
  }
  ~SpatialBNOp() {}

  bool RunOnDevice() override {
    return true;
  }

 protected:
  bool is_test_;
  double epsilon_;
  double momentum_;
  StorageOrder order_;
  INPUT_TAGS(INPUT, SCALE, BIAS, EST_MEAN, EST_VAR);
  OUTPUT_TAGS(OUTPUT, RUNNING_MEAN, RUNNING_VAR, SAVED_MEAN, SAVED_INV_VAR);
};

template <class Context>
class SpatialBNGradientOp : public Operator<Context> {
 public:
  USE_OPERATOR_CONTEXT_FUNCTIONS;
  SpatialBNGradientOp(const OperatorDef& operator_def, Workspace* ws)
      : Operator<Context>(operator_def, ws),
        is_test_(OperatorBase::GetSingleArgument<int>(OpSchema::Arg_IsTest, 0)),
        epsilon_(OperatorBase::GetSingleArgument<float>("epsilon", 1e-5)),
        order_(StringToStorageOrder(
            OperatorBase::GetSingleArgument<string>("order", "NCHW"))) {
    CAFFE_ENFORCE(InputSize() == 5);
    CAFFE_ENFORCE(OutputSize() == 3);
  }
  ~SpatialBNGradientOp() {}

  bool RunOnDevice() override {
    return true;
  }

 protected:
  bool is_test_;
  double epsilon_;
  StorageOrder order_;

  INPUT_TAGS(INPUT, SCALE, OUTPUT_GRAD, SAVED_MEAN, SAVED_INV_VAR);
  OUTPUT_TAGS(INPUT_GRAD, SCALE_GRAD, BIAS_GRAD);
};

} // namespace caffe2

#endif // CAFFE2_OPERATORS_SPATIAL_BATCH_NORM_OP_H_

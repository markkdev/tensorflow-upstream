/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#ifndef TENSORFLOW_COMPILER_XLA_SERVICE_GPU_TRIANGULAR_SOLVE_THUNK_H_
#define TENSORFLOW_COMPILER_XLA_SERVICE_GPU_TRIANGULAR_SOLVE_THUNK_H_

#include "absl/types/optional.h"
#include "tensorflow/compiler/xla/service/buffer_assignment.h"
#include "tensorflow/compiler/xla/service/gpu/buffer_allocations.h"
#include "tensorflow/compiler/xla/service/gpu/gpu_executable.h"
#include "tensorflow/compiler/xla/service/gpu/hlo_execution_profiler.h"
#include "tensorflow/compiler/xla/service/gpu/thunk.h"
#include "tensorflow/compiler/xla/service/hlo_instruction.h"
#include "tensorflow/compiler/xla/types.h"
#include "tensorflow/compiler/xla/xla_data.pb.h"
#include "tensorflow/core/lib/core/status.h"
#include "tensorflow/core/platform/stream_executor_no_cuda.h"
#include "tensorflow/stream_executor/blas.h"

namespace xla {
namespace gpu {

// This class stores everything that StreamExecutor needs to launch a triangular
// solve (BlasTrsm). It is generated by IrEmitter.
//
// Thread-compatible.
class TriangularSolveThunk : public Thunk {
 public:
  TriangularSolveThunk(const TriangularSolveOptions& options,
                       const BufferAllocation::Slice& a_buffer,
                       const BufferAllocation::Slice& b_buffer,
                       PrimitiveType type, int64 batch_size, int64 m, int64 n,
                       int64 a_batch_stride, int64 b_batch_stride,
                       const HloInstruction* hlo);

  TriangularSolveThunk(const TriangularSolveThunk&) = delete;
  TriangularSolveThunk& operator=(const TriangularSolveThunk&) = delete;

  Status ExecuteOnStream(const BufferAllocations& buffer_allocations,
                         se::Stream* stream, const RunId& run_id,
                         HloExecutionProfiler* profiler) override;

 private:
  const se::blas::UpperLower uplo_;
  const se::blas::Side side_;
  const se::blas::Diagonal unit_diagonal_;
  se::blas::Transpose transpose_a_;

  const BufferAllocation::Slice a_buffer_;
  const BufferAllocation::Slice b_buffer_;

  const PrimitiveType type_;
  const int64 batch_size_;
  const int64 m_;
  const int64 n_;
  const int64 a_batch_stride_;
  const int64 b_batch_stride_;
};

}  // namespace gpu
}  // namespace xla

#endif  // TENSORFLOW_COMPILER_XLA_SERVICE_GPU_TRIANGULAR_SOLVE_THUNK_H_

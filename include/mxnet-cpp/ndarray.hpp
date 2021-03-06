/*!
 *  Copyright (c) 2016 by Contributors
 * \file ndarray.hpp
 * \brief implementation of the ndarray
 * \author Zhang Chen, Chuntao Hong
 */

#ifndef MXNETCPP_NDARRAY_HPP
#define MXNETCPP_NDARRAY_HPP

#include <map>
#include <string>
#include <vector>
#include "mxnet-cpp/logging.h"
#include "mxnet-cpp/ndarray.h"
#include "mxnet-cpp/operator.h"

namespace mxnet {
namespace cpp {

NDArray::NDArray() {
  NDArrayHandle handle;
  CHECK_EQ(MXNDArrayCreateNone(&handle), 0);
  blob_ptr_ = std::make_shared<NDBlob>(handle);
}
NDArray::NDArray(const NDArrayHandle &handle) {
  blob_ptr_ = std::make_shared<NDBlob>(handle);
}
NDArray::NDArray(const std::vector<mx_uint> &shape, const Context &context,
                 bool delay_alloc) {
  NDArrayHandle handle;
  CHECK_EQ(MXNDArrayCreate(shape.data(), shape.size(), context.GetDeviceType(),
                           context.GetDeviceId(), delay_alloc, &handle),
           0);
  blob_ptr_ = std::make_shared<NDBlob>(handle);
}
NDArray::NDArray(const Shape &shape, const Context &context, bool delay_alloc) {
  NDArrayHandle handle;
  CHECK_EQ(MXNDArrayCreate(shape.data(), shape.ndim(), context.GetDeviceType(),
                           context.GetDeviceId(), delay_alloc, &handle),
           0);
  blob_ptr_ = std::make_shared<NDBlob>(handle);
}
NDArray::NDArray(const mx_float *data, size_t size) {
  NDArrayHandle handle;
  CHECK_EQ(MXNDArrayCreateNone(&handle), 0);
  MXNDArraySyncCopyFromCPU(handle, data, size);
  blob_ptr_ = std::make_shared<NDBlob>(handle);
}
NDArray::NDArray(const mx_float *data, const Shape &shape,
                 const Context &context) {
  NDArrayHandle handle;
  CHECK_EQ(MXNDArrayCreate(shape.data(), shape.ndim(), context.GetDeviceType(),
                           context.GetDeviceId(), false, &handle),
           0);
  MXNDArraySyncCopyFromCPU(handle, data, shape.Size());
  blob_ptr_ = std::make_shared<NDBlob>(handle);
}
NDArray::NDArray(const std::vector<mx_float> &data, const Shape &shape,
                 const Context &context) {
  NDArrayHandle handle;
  CHECK_EQ(MXNDArrayCreate(shape.data(), shape.ndim(), context.GetDeviceType(),
                           context.GetDeviceId(), false, &handle),
           0);
  MXNDArraySyncCopyFromCPU(handle, data.data(), shape.Size());
  blob_ptr_ = std::make_shared<NDBlob>(handle);
}
NDArray::NDArray(const std::vector<mx_float> &data) {
  NDArrayHandle handle;
  CHECK_EQ(MXNDArrayCreateNone(&handle), 0);
  MXNDArraySyncCopyFromCPU(handle, data.data(), data.size());
  blob_ptr_ = std::make_shared<NDBlob>(handle);
}

NDArray NDArray::operator+(mx_float scalar) {
  NDArray ret;
  Operator("_plus_scalar")(*this, scalar).Invoke(ret);
  return ret;
}
NDArray NDArray::operator-(mx_float scalar) {
  NDArray ret;
  Operator("_minus_scalar")(*this, scalar).Invoke(ret);
  return ret;
}
NDArray NDArray::operator*(mx_float scalar) {
  NDArray ret;
  Operator("_mul_scalar")(*this, scalar).Invoke(ret);
  return ret;
}
NDArray NDArray::operator/(mx_float scalar) {
  NDArray ret;
  Operator("_div_scalar")(*this, scalar).Invoke(ret);
  return ret;
}
NDArray NDArray::operator+(const NDArray &rhs) {
  NDArray ret;
  Operator("_plus")(*this, rhs).Invoke(ret);
  return ret;
}
NDArray NDArray::operator-(const NDArray &rhs) {
  NDArray ret;
  Operator("_minus")(*this, rhs).Invoke(ret);
  return ret;
}
NDArray NDArray::operator*(const NDArray &rhs) {
  NDArray ret;
  Operator("_mul")(*this, rhs).Invoke(ret);
  return ret;
}
NDArray NDArray::operator/(const NDArray &rhs) {
  NDArray ret;
  Operator("_div")(*this, rhs).Invoke(ret);
  return ret;
}
NDArray &NDArray::operator=(mx_float scalar) {
  Operator("_set_value")(scalar).Invoke(*this);
  return *this;
}
NDArray &NDArray::operator+=(mx_float scalar) {
  Operator("_plus_scalar")(*this, scalar).Invoke(*this);
  return *this;
}
NDArray &NDArray::operator-=(mx_float scalar) {
  Operator("_minus_scalar")(*this, scalar).Invoke(*this);
  return *this;
}
NDArray &NDArray::operator*=(mx_float scalar) {
  Operator("_mul_scalar")(*this, scalar).Invoke(*this);
  return *this;
}
NDArray &NDArray::operator/=(mx_float scalar) {
  Operator("_div_scalar")(*this, scalar).Invoke(*this);
  return *this;
}
NDArray &NDArray::operator+=(const NDArray &rhs) {
  Operator("_plus")(*this, rhs).Invoke(*this);
  return *this;
}
NDArray &NDArray::operator-=(const NDArray &rhs) {
  Operator("_minus")(*this, rhs).Invoke(*this);
  return *this;
}
NDArray &NDArray::operator*=(const NDArray &rhs) {
  Operator("_mul")(*this, rhs).Invoke(*this);
  return *this;
}
NDArray &NDArray::operator/=(const NDArray &rhs) {
  Operator("_div")(*this, rhs).Invoke(*this);
  return *this;
}

NDArray NDArray::ArgmaxChannel() {
  NDArray ret;
  Operator("argmax_channel")(*this).Invoke(ret);
  return ret;
}

void NDArray::SyncCopyFromCPU(const mx_float *data, size_t size) {
  MXNDArraySyncCopyFromCPU(blob_ptr_->handle_, data, size);
}
void NDArray::SyncCopyFromCPU(const std::vector<mx_float> &data) {
  MXNDArraySyncCopyFromCPU(blob_ptr_->handle_, data.data(), data.size());
}
void NDArray::SyncCopyToCPU(mx_float *data, size_t size) {
  MXNDArraySyncCopyToCPU(blob_ptr_->handle_, data, size > 0 ? size : Size());
}
void NDArray::SyncCopyToCPU(std::vector<mx_float> *data, size_t size) {
  size = size > 0 ? size : Size();
  data->resize(size);
  MXNDArraySyncCopyToCPU(blob_ptr_->handle_, data->data(), size);
}
NDArray NDArray::Copy(const Context &ctx) const {
  NDArray ret(GetShape(), ctx);
  Operator("_copyto")(*this).Invoke(ret);
  return ret;
}
NDArray NDArray::CopyTo(NDArray * other) const {
  Operator("_copyto")(*this).Invoke(*other);
  return *other;
}
NDArray NDArray::Slice(mx_uint begin, mx_uint end) const {
  NDArrayHandle handle;
  CHECK_EQ(MXNDArraySlice(GetHandle(), begin, end, &handle), 0);
  return NDArray(handle);
}
NDArray NDArray::Reshape(const Shape &new_shape) const {
  NDArrayHandle handle;
  std::vector<int> dims(new_shape.ndim());
  for (index_t i = 0; i < new_shape.ndim(); ++i) {
    dims[i] = new_shape[i];
  }
  new_shape.data();
  CHECK_EQ(
      MXNDArrayReshape(GetHandle(), new_shape.ndim(), dims.data(), &handle), 0);
  return NDArray(handle);
}
void NDArray::WaitToRead() const {
  CHECK_EQ(MXNDArrayWaitToRead(blob_ptr_->handle_), 0);
}
void NDArray::WaitToWrite() {
  CHECK_EQ(MXNDArrayWaitToWrite(blob_ptr_->handle_), 0);
}
void NDArray::WaitAll() { CHECK_EQ(MXNDArrayWaitAll(), 0); }
void NDArray::SampleGaussian(mx_float mu, mx_float sigma, NDArray *out) {
  Operator("_sample_normal")(mu, sigma).Invoke(*out);
}
void NDArray::SampleUniform(mx_float begin, mx_float end, NDArray *out) {
  Operator("_sample_uniform")(begin, end).Invoke(*out);
}
void NDArray::Load(const std::string &file_name,
                   std::vector<NDArray> *array_list,
                   std::map<std::string, NDArray> *array_map) {
  mx_uint out_size, out_name_size;
  NDArrayHandle *out_arr;
  const char **out_names;
  CHECK_EQ(MXNDArrayLoad(file_name.c_str(), &out_size, &out_arr, &out_name_size,
                         &out_names),
           0);
  if (array_list != nullptr) {
    for (mx_uint i = 0; i < out_size; ++i) {
      array_list->push_back(NDArray(out_arr[i]));
    }
  }
  if (array_map != nullptr && out_name_size > 0) {
    CHECK_EQ(out_name_size, out_size);
    for (mx_uint i = 0; i < out_size; ++i) {
      (*array_map)[out_names[i]] = NDArray(out_arr[i]);
    }
  }
}
std::map<std::string, NDArray> NDArray::LoadToMap(
    const std::string &file_name) {
  std::map<std::string, NDArray> array_map;
  mx_uint out_size, out_name_size;
  NDArrayHandle *out_arr;
  const char **out_names;
  CHECK_EQ(MXNDArrayLoad(file_name.c_str(), &out_size, &out_arr, &out_name_size,
                         &out_names),
           0);
  if (out_name_size > 0) {
    CHECK_EQ(out_name_size, out_size);
    for (mx_uint i = 0; i < out_size; ++i) {
      array_map[out_names[i]] = NDArray(out_arr[i]);
    }
  }
  return array_map;
}
std::vector<NDArray> NDArray::LoadToList(const std::string &file_name) {
  std::vector<NDArray> array_list;
  mx_uint out_size, out_name_size;
  NDArrayHandle *out_arr;
  const char **out_names;
  CHECK_EQ(MXNDArrayLoad(file_name.c_str(), &out_size, &out_arr, &out_name_size,
                         &out_names),
           0);
  for (mx_uint i = 0; i < out_size; ++i) {
    array_list.push_back(NDArray(out_arr[i]));
  }
  return array_list;
}
void NDArray::Save(const std::string &file_name,
                   const std::map<std::string, NDArray> &array_map) {
  std::vector<NDArrayHandle> args;
  std::vector<const char *> keys;
  for (const auto &t : array_map) {
    args.push_back(t.second.GetHandle());
    keys.push_back(t.first.c_str());
  }
  CHECK_EQ(
      MXNDArraySave(file_name.c_str(), args.size(), args.data(), keys.data()),
      0);
}
void NDArray::Save(const std::string &file_name,
                   const std::vector<NDArray> &array_list) {
  std::vector<NDArrayHandle> args;
  for (const auto &t : array_list) {
    args.push_back(t.GetHandle());
  }
  CHECK_EQ(MXNDArraySave(file_name.c_str(), args.size(), args.data(), nullptr),
           0);
}

size_t NDArray::Offset(size_t h, size_t w) const {
  return (h * GetShape()[1]) + w;
}

size_t NDArray::Offset(size_t c, size_t h, size_t w) const {
  auto const shape = GetShape();
  return h * shape[0] * shape[2] + w * shape[0] + c;
}

mx_float NDArray::At(size_t h, size_t w) const {
  return GetData()[Offset(h, w)];
}

mx_float NDArray::At(size_t c, size_t h, size_t w) const {
  return GetData()[Offset(c, h, w)];
}

size_t NDArray::Size() const {
  size_t ret = 1;
  for (auto &i : GetShape()) ret *= i;
  return ret;
}

std::vector<mx_uint> NDArray::GetShape() const {
  const mx_uint *out_pdata;
  mx_uint out_dim;
  MXNDArrayGetShape(blob_ptr_->handle_, &out_dim, &out_pdata);
  std::vector<mx_uint> ret;
  for (mx_uint i = 0; i < out_dim; ++i) {
    ret.push_back(out_pdata[i]);
  }
  return ret;
}

const mx_float *NDArray::GetData() const {
  mx_float *ret;
  CHECK_NE(GetContext().GetDeviceType(), DeviceType::kGPU);
  MXNDArrayGetData(blob_ptr_->handle_, &ret);
  return ret;
}
Context NDArray::GetContext() const {
  int out_dev_type;
  int out_dev_id;
  MXNDArrayGetContext(blob_ptr_->handle_, &out_dev_type, &out_dev_id);
  return Context((DeviceType)out_dev_type, out_dev_id);
}
}  // namespace cpp
}  // namespace mxnet

#endif  // MXNETCPP_NDARRAY_HPP

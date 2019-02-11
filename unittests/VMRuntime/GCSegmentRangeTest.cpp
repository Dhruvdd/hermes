/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the LICENSE
 * file in the root directory of this source tree.
 */
// This test uses NCGen files, so should only be enabled for that build.
#ifdef HERMESVM_GC_NONCONTIG_GENERATIONAL

#include "gtest/gtest.h"

#include "hermes/VM/AlignedHeapSegment.h"
#include "hermes/VM/GCSegmentRange.h"

#include <vector>

using namespace hermes;
using namespace hermes::vm;

namespace {

struct GCSegmentRangeTest : public ::testing::Test {
  GCSegmentRangeTest();
  AlignedHeapSegment newSegment();

 private:
  std::unique_ptr<StorageProvider> provider_;
};

GCSegmentRangeTest::GCSegmentRangeTest()
    : provider_{StorageProvider::mmapProvider()} {}

AlignedHeapSegment GCSegmentRangeTest::newSegment() {
  AlignedHeapSegment seg{provider_.get()};
  EXPECT_TRUE(seg);

  return seg;
}

TEST_F(GCSegmentRangeTest, EmptyConsumable) {
  auto range =
      GCSegmentRange::fromConsumable<AlignedHeapSegment *>(nullptr, nullptr);
  EXPECT_EQ(nullptr, range->next());
}

TEST_F(GCSegmentRangeTest, SingletonConsumable) {
  auto seg = newSegment();
  auto range = GCSegmentRange::singleton(&seg);

  EXPECT_EQ(&seg, range->next());
  EXPECT_EQ(nullptr, range->next());
}

TEST_F(GCSegmentRangeTest, IterConsumable) {
  constexpr size_t NUM = 10;
  std::vector<AlignedHeapSegment> segs;

  for (size_t i = 0; i < NUM; ++i) {
    segs.emplace_back(newSegment());
  }

  auto range = GCSegmentRange::fromConsumable(segs.begin(), segs.end());

  for (size_t i = 0; i < NUM; ++i) {
    EXPECT_EQ(&segs[i], range->next()) << "Mismatch at " << i;
  }

  EXPECT_EQ(nullptr, range->next());
}

} // namespace

#endif // HERMESVM_GC_NONCONTIG_GENERATIONAL

//
//  LMCShader.metal
//  MetalCourseEx2-macOS
//
//  Created by Barak Yoresh on 26/03/2019.
//  Copyright © 2019 Apple. All rights reserved.
//

#include <metal_stdlib>

#import "LMCSharedTypes.h"

struct VertexOut {
  float4 position [[ position ]];
  float2 texCoord;
};

vertex VertexOut vertexFunction(const device Vertex *verticesIn [[ buffer(0) ]],
                                constant matrix_float4x4 &aspectRatioMatrix [[buffer(1)]],
                                uint vid [[ vertex_id ]]) {
  VertexOut out;
  out.position = aspectRatioMatrix * float4(verticesIn[vid].position, 0.0, 1.0); // TODO: why 1.0?
  out.texCoord = verticesIn[vid].texCoord;
  return out;
}

constexpr metal::sampler s(metal::address::clamp_to_zero);

fragment float4 fragmentFunction(VertexOut in [[ stage_in ]],
                                 metal::texture2d<float> texture [[ texture(0) ]]) {
  return texture.sample(s, in.texCoord);
}

using namespace metal;

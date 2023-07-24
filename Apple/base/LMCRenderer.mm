// Copyright (c) 2019 Lightricks. All rights reserved.
// Created by Barak Yoresh.

#import <simd/simd.h>
#import <MetalKit/MetalKit.h>

#import "LMCRenderer.h"

#import "LMCSharedTypes.h"
#import "LMCUtils.h"

NS_ASSUME_NONNULL_BEGIN

@interface LMCRenderer ()

/// Device representing the GPU.
@property (readonly, nonatomic) id<MTLDevice> device;

/// Queue of command buffers delivered to the GPU for rendering.
@property (readonly, nonatomic) id<MTLCommandQueue> commandQueue;

/// TODO: doc
@property (readonly, nonatomic) id<MTLRenderPipelineState> pipelineState;

@property (readonly, nonatomic) TextureProvider textureProvider;

@end

@implementation LMCRenderer

/// Initialize with the MetalKit view from which we'll obtain our Metal device
- (nonnull instancetype)initWithMetalKitView:(nonnull MTKView *)mtkView
                             textureProvider:(TextureProvider)textureProvider{
  if (self = [super init]) {
    _device = mtkView.device;
    _commandQueue = [self.device newCommandQueue];
    mtkView.clearColor = MTLClearColorMake(0.75, 0.75, 0.75, 1);
    _pipelineState = [LMCRenderer createRenderPipeline:self.device];
    _textureProvider = textureProvider;
  }
  return self;
}

/// TODO: doc
+ (id<MTLRenderPipelineState>)createRenderPipeline:(id<MTLDevice>)device {
  MTLRenderPipelineDescriptor *descriptor = [MTLRenderPipelineDescriptor new];

  descriptor.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;
  id<MTLLibrary> library = [device newDefaultLibrary];
  descriptor.vertexFunction = [library newFunctionWithName:@"vertexFunction"];
  descriptor.fragmentFunction = [library newFunctionWithName:@"fragmentFunction"];
  descriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm_sRGB;
  
  NSError *error;
  id<MTLRenderPipelineState> pipelineState =
      [device newRenderPipelineStateWithDescriptor:descriptor error:&error];
  
  return pipelineState;
}

+ (id<MTLTexture>)loadTextureForDevice:(id<MTLDevice>)device View:(nonnull MTKView *)mtkView {
  auto usage = MTLTextureUsageShaderRead | MTLTextureUsageShaderWrite | MTLTextureUsageRenderTarget;
  MTKTextureLoader *textureLoader = [[MTKTextureLoader alloc] initWithDevice:device];

  NSError *error;
  id<MTLTexture> texture = [textureLoader newTextureWithName:@"sweetdreams"
#if defined(TARGET_IOS) || defined(TARGET_TVOS)
      scaleFactor:1 // TODO: view's contentsScale
#else
      scaleFactor:1 // TODO: backingScaleFactor value of the window
#endif
      bundle:nil // TODO: Media.xcassets?
      options:@{MTKTextureLoaderOptionTextureUsage: @(usage)}
      error:&error];
  return texture;
}

#pragma mark -
#pragma mark MTKViewDelegate
#pragma mark -

- (void)drawInMTKView:(nonnull MTKView *)view {
  static const struct Vertex kTriangle[] = {
    { {-1.0,  1.0}, {0, 0} },
    { {-1.0, -1.0}, {0, 1} },
    { { 1.0,  1.0}, {1, 0} },
    { { 1.0, -1.0}, {1, 1} }
  };

  auto texture = self.textureProvider();
  
  matrix_float4x4 matrix =
      lmc_modelViewProjection(view.bounds.size, texture.width, texture.height);
  id <MTLBuffer> matrixBuffer =
      [self.device newBufferWithBytes:&matrix length:sizeof(matrix)
                              options:MTLResourceCPUCacheModeDefaultCache];
  
  id <MTLCommandBuffer> commandBuffer = [self.commandQueue commandBuffer];
  
  id <MTLRenderCommandEncoder> renderEncoder =
      [commandBuffer renderCommandEncoderWithDescriptor:view.currentRenderPassDescriptor];
  
  [renderEncoder setRenderPipelineState:self.pipelineState];
  [renderEncoder setVertexBytes:&kTriangle length:sizeof(kTriangle) atIndex:0];
  [renderEncoder setVertexBuffer:matrixBuffer offset:0 atIndex:1];
  [renderEncoder setFragmentTexture:texture atIndex:0];
  [renderEncoder drawPrimitives:MTLPrimitiveTypeTriangleStrip vertexStart:0 vertexCount:4];
  [renderEncoder endEncoding];
  
  [commandBuffer presentDrawable:view.currentDrawable];
  [commandBuffer commit];
}

- (void)mtkView:(MTKView *)view drawableSizeWillChange:(CGSize)size {
}

@end

NS_ASSUME_NONNULL_END

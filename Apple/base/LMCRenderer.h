// Copyright (c) 2019 Lightricks. All rights reserved.
// Created by Barak Yoresh.

#import <MetalKit/MetalKit.h>

typedef _Nonnull id<MTLTexture> (^TextureProvider)();

NS_ASSUME_NONNULL_BEGIN

// Our platform independent renderer class
@interface LMCRenderer : NSObject <MTKViewDelegate>

- (instancetype)initWithMetalKitView:(nonnull MTKView *)mtkView
                     textureProvider:(TextureProvider)textureProvider;

@end

NS_ASSUME_NONNULL_END

//
//  LMCUtils.m
//  MetalCourseEx2
//
//  Created by Barak Yoresh on 27/03/2019.
//  Copyright © 2019 Apple. All rights reserved.
//

#import <SceneKit/SceneKit.h>

// Creates and returns a 4x4 model view projection matrix correcting the aspect ratio of a
// normalized quad to that of a texture with \c textureWidth and \c textureHeight aspect fitted to
// \c drawableSize.
static inline matrix_float4x4 lmc_modelViewProjection(CGSize drawableSize, CGFloat textureWidth,
                                                      CGFloat textureHeight) {
  CGFloat widthRatio = drawableSize.width / textureWidth;
  CGFloat heightRatio = drawableSize.height / textureHeight;

  CGFloat fittedWidth = textureWidth * MIN(widthRatio, heightRatio);
  CGFloat fittedHeight = textureHeight * MIN(widthRatio, heightRatio);

  SCNMatrix4 modelViewProjection = SCNMatrix4MakeScale(fittedWidth / drawableSize.width,
                                                       fittedHeight / drawableSize.height, 1);

  return SCNMatrix4ToMat4(modelViewProjection);
}

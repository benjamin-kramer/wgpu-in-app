//
//  LMCSharedTypes.h
//  MetalCourseEx2
//
//  Created by Barak Yoresh on 26/03/2019.
//  Copyright © 2019 Apple. All rights reserved.
//

#ifndef sharedTypes_h
#define sharedTypes_h

#import "MTBSIMDExtensions.metal.h"

struct Vertex {
  vector_float2 position;
  vector_float2 texCoord;
};

#endif /* LMCSharedTypes_h */

//
//  ViewController.swift
//
//  Created by LiJinlei on 2021/9/10.
//

import UIKit
import MetalKit

class ViewController: UIViewController {
    @IBOutlet var metalV: MTKView!
    var wgpuCanvas: OpaquePointer?

    var renderer: LMCRenderer!
    var texture: MTLTexture!
    
    lazy var displayLink: CADisplayLink = {
        CADisplayLink.init(target: self, selector: #selector(enterFrame))
    }()
    
    override func viewDidLoad() {
        super.viewDidLoad()
       
        self.displayLink.add(to: .current, forMode: .default)
        self.displayLink.isPaused = true
    }
    
    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(animated)
        self.view.backgroundColor = .white

        if wgpuCanvas == nil {
            let maximumFrames = UIScreen.main.maximumFramesPerSecond

            let device = MTLCreateSystemDefaultDevice()!
            metalV.device = device

            let commandQueue = device.makeCommandQueue()!
            let devicePtr =
              UnsafeMutableRawPointer(mutating: Unmanaged.passUnretained(device).toOpaque())
            let queuePtr = UnsafeMutableRawPointer(mutating: Unmanaged.passUnretained(commandQueue).toOpaque())

            let descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .bgra8Unorm_srgb,
                                                                      width: 1122, height: 1122,
                                                                      mipmapped: false)
            descriptor.usage = [.shaderRead, .shaderWrite, .renderTarget]
            texture = device.makeTexture(descriptor: descriptor)

            renderer = .init(metalKitView: metalV, textureProvider: {
              return self.texture
            })
            metalV.delegate = renderer

            let texturePtr =
                UnsafeMutableRawPointer(mutating: Unmanaged.passUnretained(texture).toOpaque())

            let viewObj = ios_view_obj(
              mtl_device_ptr: devicePtr,
              mtl_command_queue_ptr: queuePtr,
              mtl_texture_ptr: texturePtr,
              callback_to_swift: callback_to_swift
            )
            
            wgpuCanvas = create_wgpu_canvas(viewObj)
        }
        self.displayLink.isPaused = false
    }
    
    override func viewWillDisappear(_ animated: Bool) {
        super.viewWillDisappear(animated)
        displayLink.isPaused = true
    }
    
    @objc func enterFrame() {
        guard let canvas = self.wgpuCanvas else {
            return
        }
        // call rust
        enter_frame(canvas)
        metalV.setNeedsDisplay()
    }
    
    @IBAction func changeExample(sender: UISegmentedControl) {
        guard let canvas = self.wgpuCanvas else {
            return
        }
        let index = sender.selectedSegmentIndex
        change_example(canvas, Int32(index))
        enter_frame(canvas)
        metalV.setNeedsDisplay()
    }

}

func callback_to_swift(arg: Int32) {
    DispatchQueue.main.async {
        switch arg {
        case 0:
            print("wgpu canvas created!")
            break
        case 1:
            print("canvas enter frame")
            break
            
        default:
            break
        }
    }
    
}

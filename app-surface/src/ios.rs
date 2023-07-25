use libc::c_void;
use std::marker::Sync;
use std::sync::Arc;

use wgpu_hal as hal;
use metal;
use foreign_types::ForeignType;
use wgpu_types as wgt;


#[repr(C)]
pub struct IOSViewObj {
    pub mtl_device_ptr: *mut c_void,
    pub mtl_command_queue_ptr: *mut c_void,
    pub mtl_texture_ptr: *mut c_void,

    pub callback_to_swift: extern "C" fn(arg: i32),
}

pub struct AppSurface {
    pub adapter: wgpu::Adapter,
    pub device: Arc<wgpu::Device>,
    pub queue: Arc<wgpu::Queue>,
    pub texture: wgpu::Texture,

    pub callback_to_app: Option<extern "C" fn(arg: i32)>,
    pub temporary_directory: &'static str,
    pub library_directory: &'static str,
}

unsafe impl Sync for AppSurface {}

impl AppSurface {
    pub fn new(obj: IOSViewObj) -> Self {
        // hook up rust logging
        env_logger::init();

        let mtl_device;
        unsafe {
            let device_raw_ptr = metal::Device::from_ptr(obj.mtl_device_ptr as *mut metal::MTLDevice);
            mtl_device = hal::metal::Device::device_from_raw(device_raw_ptr, wgpu::Features::empty());
        };
    
        let mtl_command_queue;
        unsafe {
            let command_queue_raw_ptr = 
                metal::CommandQueue::from_ptr(obj.mtl_command_queue_ptr as *mut metal::MTLCommandQueue);
            mtl_command_queue = hal::metal::Queue::queue_from_raw(command_queue_raw_ptr);
        } 
    
        let instance_descriptor = wgpu::InstanceDescriptor::default();
        let wgpu_instance = wgpu::Instance::new(instance_descriptor);
    
        let options = wgpu::RequestAdapterOptions::default();
        let adapter = pollster::block_on(wgpu_instance.request_adapter(&options)).expect("Failed to get adapter!");
        
        let adapter_result;
        unsafe {
            adapter_result = adapter.create_device_from_hal(hal::OpenDevice::<hal::api::Metal> {
                device: mtl_device,
                queue: mtl_command_queue
            }, &wgpu::DeviceDescriptor {
                label: None,
                features: wgpu::Features::empty(),
                limits: wgpu::Limits::default()
            }, None)
        }
        let (device, queue) = adapter_result.unwrap();


        let texture;
        unsafe {
            let texture_ptr = metal::Texture::from_ptr(obj.mtl_texture_ptr as *mut metal::MTLTexture);
            let mtl_texture = hal::metal::Device::texture_from_raw(
                texture_ptr, 
                wgt::TextureFormat::Rgba8Unorm,
                metal::MTLTextureType::D2,
                1,
                1, 
                wgpu_hal::CopyExtent{width: 1, height: 1, depth: 1}
            );

        
            texture = device.create_texture_from_hal::<wgpu_hal::metal::Api>(mtl_texture, &wgpu::TextureDescriptor {
                label: wgpu_hal::Label::Some("someTexture"),
                size: wgt::Extent3d { width: 1122, height: 1122, depth_or_array_layers: 3 },
                mip_level_count: 1,
                sample_count: 1,
                dimension: wgt::TextureDimension::D2,
                format: wgt::TextureFormat::Bgra8UnormSrgb,
                usage: wgt::TextureUsages::all(),
                view_formats: &[wgt::TextureFormat::Bgra8UnormSrgb]
            });
        }

        AppSurface {
            adapter,
            device: Arc::new(device),
            queue: Arc::new(queue),
            texture: texture,
            
            callback_to_app: Some(obj.callback_to_swift),
            temporary_directory: "",
            library_directory: "",
        }
    }

    pub fn get_current_view(
        &self,
        view_format: Option<wgpu::TextureFormat>,
    ) -> wgpu::TextureView {

        let view = self.texture.create_view(&wgpu::TextureViewDescriptor {
            label: Some("frame texture view"),
            format: if view_format.is_none() {
                Some(wgpu::TextureFormat::Bgra8UnormSrgb)
            } else {
                view_format
            },
            dimension: Some(wgt::TextureViewDimension::D2),
            ..Default::default()
        });
        return view;
    }
}
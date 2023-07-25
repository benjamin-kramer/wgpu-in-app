// use wgpu::{Instance, Surface};
//
// mod touch;
// pub use touch::*;

// #[cfg_attr(target_os = "android", path = "android.rs")]
#[cfg_attr(target_os = "ios", path = "ios.rs")]
mod app_surface;
pub use app_surface::*;

// #[repr(C)]
// #[derive(Debug)]
// pub struct ViewSize {
//     pub width: u32,
//     pub height: u32,
// }
//
// #[allow(unused)]
// async fn request_device(
//     instance: &Instance,
//     surface: &Surface,
// ) -> (wgpu::Adapter, wgpu::Device, wgpu::Queue) {
//     let adapter = instance
//         .request_adapter(&wgpu::RequestAdapterOptions {
//             power_preference: wgpu::util::power_preference_from_env()
//                 .unwrap_or(wgpu::PowerPreference::HighPerformance),
//             force_fallback_adapter: false,
//             compatible_surface: Some(surface),
//         })
//         .await
//         .expect("No suitable GPU adapters found on the system!");
//     let adapter_info = adapter.get_info();
//     println!("Using {} ({:?})", adapter_info.name, adapter_info.backend);
//     let base_dir = std::env::var("CARGO_MANIFEST_DIR");
//     let _trace_path = if let Ok(base_dir) = base_dir {
//         Some(std::path::PathBuf::from(&base_dir).join("WGPU_TRACE_ERROR"))
//     } else {
//         None
//     };
//     let res = adapter
//         .request_device(
//             &wgpu::DeviceDescriptor {
//                 label: None,
//                 features: adapter.features(),
//                 limits: adapter.limits(),
//             },
//             None,
//         )
//         .await;
//     match res {
//         Err(err) => {
//             panic!("request_device failed: {err:?}");
//         }
//         Ok(tuple) => (adapter, tuple.0, tuple.1),
//     }
// }

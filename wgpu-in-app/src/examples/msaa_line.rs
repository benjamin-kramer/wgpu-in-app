//! copy from wgpu's example
//!
//! The parts of this example enabling MSAA are:
//! *    The render pipeline is created with a sample_count > 1.
//! *    A new texture with a sample_count > 1 is created and set as the color_attachment instead of the swapchain.
//! *    The swapchain is now specified as a resolve_target.
//!
//! The parts of this example enabling LineList are:
//! *   Set the primitive_topology to PrimitiveTopology::LineList.
//! *   Vertices and Indices describe the two points that make up a line.

use super::Example;
use app_surface::AppSurface;

use std::{borrow::Cow, iter};

use bytemuck::{Pod, Zeroable};
use wgpu::util::DeviceExt;

#[repr(C)]
#[derive(Clone, Copy, Pod, Zeroable)]
struct Vertex {
    _pos: [f32; 2],
    _color: [f32; 4],
}

pub struct MSAALine {
    bundle: wgpu::RenderBundle,
    shader: wgpu::ShaderModule,
    pipeline_layout: wgpu::PipelineLayout,
    multisampled_framebuffer: wgpu::TextureView,
    vertex_buffer_list: Vec<wgpu::Buffer>,
    vertex_count: u32,
    sample_count: u32,
    rebuild_bundle: bool,
}

impl MSAALine {
    pub fn new(app_surface: &mut AppSurface) -> Self {
        // Only srgb format can show real MSAA effect on metal backend.
        
        // Note: we use an sRGB pixelFormat in advance to support this feature.
        // let msaa_format = app_surface.config.format.add_srgb_suffix();
        // app_surface.sdq.update_config_format(msaa_format);

        let device = &app_surface.device;

        let sample_flags = app_surface
            .adapter
            .get_texture_format_features(wgpu::TextureFormat::Bgra8UnormSrgb)
            .flags;
        let sample_count = {
            if sample_flags.contains(wgpu::TextureFormatFeatureFlags::MULTISAMPLE_X8) {
                8
            } else if sample_flags.contains(wgpu::TextureFormatFeatureFlags::MULTISAMPLE_X4) {
                4
            } else if sample_flags.contains(wgpu::TextureFormatFeatureFlags::MULTISAMPLE_X2) {
                2
            } else {
                1
            }
        };

        let shader = device.create_shader_module(wgpu::ShaderModuleDescriptor {
            label: None,
            source: wgpu::ShaderSource::Wgsl(Cow::Borrowed(include_str!(
                "../../wgsl_shader/msaa_line.wgsl"
            ))),
        });

        let pipeline_layout = device.create_pipeline_layout(&wgpu::PipelineLayoutDescriptor {
            label: None,
            bind_group_layouts: &[],
            push_constant_ranges: &[],
        });

        let multisampled_framebuffer = Self::create_multisampled_framebuffer(
            device, 
            app_surface.texture.width(),
            app_surface.texture.height(),
            sample_count
        );

        let mut vertex_data_list = vec![];

        let max = 50;
        let list_count = 5;

        let step = 2.0 / (list_count as f32 + 1.0);
        let mut x = -1.0;
        for _ in 0..list_count {
            let mut vertex_data = vec![];
            x += step;
            for i in 0..max {
                let percent = i as f32 / max as f32;
                let (sin, cos) = (percent * 2.0 * std::f32::consts::PI).sin_cos();
                vertex_data.push(Vertex {
                    _pos: [x, 0.0],
                    _color: [1.0, -sin, cos, 1.0],
                });
                vertex_data.push(Vertex {
                    _pos: [1.0 * cos, 1.0 * sin],
                    _color: [sin, -cos, 1.0, 1.0],
                });
            }
            vertex_data_list.push(vertex_data);
        }

        let mut vertex_buffer_list: Vec<wgpu::Buffer> = vec![];
        for item_data in vertex_data_list.iter().take(list_count) {
            let vertex_buffer = device.create_buffer_init(&wgpu::util::BufferInitDescriptor {
                label: Some("Vertex Buffer"),
                contents: bytemuck::cast_slice(item_data),
                usage: wgpu::BufferUsages::VERTEX,
            });
            vertex_buffer_list.push(vertex_buffer);
        }
        let vertex_count = max as u32 * 2;

        let bundle = Self::create_bundle(
            device,
            &shader,
            &pipeline_layout,
            sample_count,
            &vertex_buffer_list,
            vertex_count,
        );

        Self {
            bundle,
            shader,
            pipeline_layout,
            multisampled_framebuffer,
            vertex_buffer_list,
            vertex_count,
            sample_count,
            rebuild_bundle: false,
        }
    }

    fn create_bundle(
        device: &wgpu::Device,
        shader: &wgpu::ShaderModule,
        pipeline_layout: &wgpu::PipelineLayout,
        sample_count: u32,
        vertex_buffer_list: &Vec<wgpu::Buffer>,
        vertex_count: u32,
    ) -> wgpu::RenderBundle {
        log::info!("sample_count: {}", sample_count);
        let pipeline = device.create_render_pipeline(&wgpu::RenderPipelineDescriptor {
            label: None,
            layout: Some(pipeline_layout),
            vertex: wgpu::VertexState {
                module: shader,
                entry_point: "vs_main",
                buffers: &[wgpu::VertexBufferLayout {
                    array_stride: std::mem::size_of::<Vertex>() as wgpu::BufferAddress,
                    step_mode: wgpu::VertexStepMode::Vertex,
                    attributes: &wgpu::vertex_attr_array![0 => Float32x2, 1 => Float32x4],
                }],
            },
            fragment: Some(wgpu::FragmentState {
                module: shader,
                entry_point: "fs_main",
                targets: &[Some(wgpu::ColorTargetState { 
                    format: wgpu::TextureFormat::Bgra8UnormSrgb,
                    blend: Some(wgpu::BlendState::REPLACE), 
                    write_mask: wgpu::ColorWrites::all() 
                })],
            }),
            primitive: wgpu::PrimitiveState {
                topology: wgpu::PrimitiveTopology::LineList,
                front_face: wgpu::FrontFace::Ccw,
                ..Default::default()
            },
            depth_stencil: None,
            multisample: wgpu::MultisampleState {
                count: sample_count,
                ..Default::default()
            },
            multiview: None,
        });
        let mut encoder =
            device.create_render_bundle_encoder(&wgpu::RenderBundleEncoderDescriptor {
                label: None,
                color_formats: &[Some(wgpu::TextureFormat::Bgra8UnormSrgb)],
                depth_stencil: None,
                sample_count,
                multiview: None,
            });
        encoder.set_pipeline(&pipeline);
        for buf in vertex_buffer_list {
            encoder.set_vertex_buffer(0, buf.slice(..));
            encoder.draw(0..vertex_count, 0..1);
        }
        // encoder.set_vertex_buffer(0, vertex_buffer_list[0].slice(..));
        // encoder.draw(0..vertex_count, 0..1);
        encoder.finish(&wgpu::RenderBundleDescriptor {
            label: Some("main"),
        })
    }

    fn create_multisampled_framebuffer(
        device: &wgpu::Device,
        width: u32,
        height: u32,
        sample_count: u32,
    ) -> wgpu::TextureView {
        let multisampled_texture_extent = wgpu::Extent3d {
            width: width,
            height: height,
            depth_or_array_layers: 1,
        };
        let multisampled_frame_descriptor = &wgpu::TextureDescriptor {
            size: multisampled_texture_extent,
            mip_level_count: 1,
            sample_count,
            dimension: wgpu::TextureDimension::D2,
            format: wgpu::TextureFormat::Bgra8UnormSrgb,
            usage: wgpu::TextureUsages::RENDER_ATTACHMENT,
            label: None,
            view_formats: &[],
        };

        device
            .create_texture(multisampled_frame_descriptor)
            .create_view(&wgpu::TextureViewDescriptor::default())
    }
}

impl Example for MSAALine {
    fn enter_frame(&mut self, app_surface: &AppSurface) {
        let device = &app_surface.device;
        let queue = &app_surface.queue;
        if self.rebuild_bundle {
            self.bundle = Self::create_bundle(
                device,
                &self.shader,
                &self.pipeline_layout,
                self.sample_count,
                &self.vertex_buffer_list,
                self.vertex_count,
            );
            self.multisampled_framebuffer = MSAALine::create_multisampled_framebuffer(
                device, 
                app_surface.texture.width(),
                app_surface.texture.height(), 
                self.sample_count
            );
            self.rebuild_bundle = false;
        }
        let view = app_surface.get_current_view(None);
        
        let mut encoder =
            device.create_command_encoder(&wgpu::CommandEncoderDescriptor { label: None });
        {
            let rpass_color_attachment = if self.sample_count == 1 {
                wgpu::RenderPassColorAttachment {
                    view: &view,
                    resolve_target: None,
                    ops: wgpu::Operations {
                        load: wgpu::LoadOp::Clear(wgpu::Color::BLACK),
                        store: true,
                    },
                }
            } else {
                wgpu::RenderPassColorAttachment {
                    view: &self.multisampled_framebuffer,
                    resolve_target: Some(&view),
                    ops: wgpu::Operations {
                        load: wgpu::LoadOp::Clear(wgpu::Color::BLACK),
                        // Storing pre-resolve MSAA data is unnecessary if it isn't used later.
                        // On tile-based GPU, avoid store can reduce your app's memory footprint.
                        store: false,
                    },
                }
            };

            encoder
                .begin_render_pass(&wgpu::RenderPassDescriptor {
                    label: None,
                    color_attachments: &[Some(rpass_color_attachment)],
                    depth_stencil_attachment: None,
                })
                .execute_bundles(iter::once(&self.bundle));
        }

        queue.submit(iter::once(encoder.finish()));
    }
}

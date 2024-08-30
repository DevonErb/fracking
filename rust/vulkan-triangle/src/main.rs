use vulkano::device::{Device, DeviceExtensions, Queue};
use vulkano::instance::{Instance, InstanceExtensions, PhysicalDevice};
use vulkano::buffer::{BufferUsage, CpuAccessibleBuffer};
use vulkano::pipeline::{GraphicsPipeline, Pipeline, viewport::Viewport};
use vulkano::framebuffer::{Framebuffer, RenderPassAbstract, Subpass};
use vulkano::image::{SwapchainImage};
use vulkano::swapchain::{SurfaceTransform, PresentMode, FullscreenExclusive, Swapchain, SwapchainCreationError};
use vulkano::sync::{GpuFuture};
use vulkano_win::VkSurfaceBuild;
use winit::{EventsLoop, WindowBuilder};

use std::sync::Arc;
use std::iter;

fn main() {
    // Create an instance
    let instance = Instance::new(None, &InstanceExtensions::none(), None).expect("Failed to create instance");

    // Create an event loop
    let mut events_loop = EventsLoop::new();

    // Create a window surface
    let surface = WindowBuilder::new().build_vk_surface(&events_loop, instance.clone()).unwrap();

    // Select a physical device
    let physical = PhysicalDevice::enumerate(&instance).next().expect("No device available");

    // Find a queue family that supports graphics and is compatible with the surface
    let queue_family = physical.queue_families()
        .find(|&q| q.supports_graphics() && surface.is_supported(q).unwrap_or(false))
        .expect("Couldn't find a graphical queue family");

    // Create a device and queues
    let (device, mut queues) = Device::new(physical, &physical.supported_features(), &DeviceExtensions::none(),
                                           [(queue_family, 0.5)].iter().cloned()).expect("Failed to create device");
    let queue = queues.next().unwrap();

    // Create a swapchain
    let (mut swapchain, images) = {
        let caps = surface.capabilities(physical).expect("Failed to get surface capabilities");
        let dimensions = caps.current_extent.unwrap_or([1024, 768]);
        let format = caps.supported_formats[0].0;
        Swapchain::new(device.clone(), surface.clone(), caps.min_image_count, format, dimensions, 1, ImageUsage::color_attachment(),
                       &queue, SurfaceTransform::Identity, alpha_mode, PresentMode::Fifo, FullscreenExclusive::Default,
                       true, ColorSpace::SrgbNonLinear).expect("Failed to create swapchain")
    };

    // Vertex shader
    mod vs {
        vulkano_shaders::shader!{
            ty: "vertex",
            src: "
                #version 450
                layout(location = 0) in vec2 position;
                void main() {
                    gl_Position = vec4(position, 0.0, 1.0);
                }
            "
        }
    }

    // Fragment shader
    mod fs {
        vulkano_shaders::shader!{
            ty: "fragment",
            src: "
                #version 450
                layout(location = 0) out vec4 f_color;
                void main() {
                    f_color = vec4(1.0, 0.0, 0.0, 1.0);
                }
            "
        }
    }

    let vs = vs::Shader::load(device.clone()).expect("Failed to create vertex shader module");
    let fs = fs::Shader::load(device.clone()).expect("Failed to create fragment shader module");

    // Create a render pass
    let render_pass = Arc::new(vulkano::single_pass_renderpass!(device.clone(),
        attachments: {
            color: {
                load: Clear,
                store: Store,
                format: swapchain.format(),
                samples: 1,
            }
        },
        pass: {
            color: [color],
            depth_stencil: {}
        }
    ).unwrap());

    // Create a graphics pipeline
    let pipeline = Arc::new(GraphicsPipeline::start()
        .vertex_input_single_buffer()
        .vertex_shader(vs.main_entry_point(), ())
        .triangle_list()
        .viewports_dynamic_scissors_irrelevant(1)
        .fragment_shader(fs.main_entry_point(), ())
        .render_pass(Subpass::from(render_pass.clone(), 0).unwrap())
        .build(device.clone())
        .unwrap());

    // Framebuffers creation
    let mut framebuffers: Option<Vec<Arc<dyn FramebufferAbstract + Send + Sync>>> = None;

    // Creating vertex buffer
    let vertex_buffer = {
        #[derive(Debug, Clone)]
        struct Vertex { position: [f32; 2] }
        impl_vertex!(Vertex, position);

        CpuAccessibleBuffer::from_iter(device.clone(), BufferUsage::all(), false, [
            Vertex { position: [-0.5, -0.5] },
            Vertex { position: [ 0.0,  0.5] },
            Vertex { position: [ 0.5, -0.25] },
        ].iter().cloned()).unwrap()
    };

    // Main loop
    loop {
        events_loop.poll_events(|_| ());

        let (image_num, suboptimal, acquire_future) =
            match vulkano::swapchain::acquire_next_image(swapchain.clone(), None) {
                Ok(r) => r,
                Err(SwapchainCreationError::OutOfDate) => {
                    // Recreate swapchain
                    continue;
                },
                Err(err) => panic!("{:?}", err)
            };

        if suboptimal {
            // Recreate swapchain
            continue;
        }

        // If framebuffers are not initialized, initialize them
        if framebuffers.is_none() {
            let new_framebuffers = Some(images.iter().map(|image| {
                Arc::new(Framebuffer::start(render_pass.clone())
                    .add(image.clone()).unwrap()
                    .build().unwrap()) as Arc<dyn FramebufferAbstract + Send + Sync>
            }).collect::<Vec<_>>());
            framebuffers = new_framebuffers;
        }

        let clear_values = vec![[0.0, 0.0, 0.0, 1.0].into()];

        let mut builder = AutoCommandBufferBuilder::primary_one_time_submit(device.clone(), queue.family()).unwrap();
        builder
            .begin_render_pass(framebuffers.as_ref().unwrap()[image_num].clone(), SubpassContents::Inline, clear_values)
            .unwrap()
            .draw(pipeline.clone(), &DynamicState::none(), vertex_buffer.clone(), (), ())
            .unwrap()
            .end_render_pass()
            .unwrap();
        let command_buffer = builder.build().unwrap();

        let future = acquire_future
            .then_execute(queue.clone(), command_buffer).unwrap()
            .then_swapchain_present(queue.clone(), swapchain.clone(), image_num)
            .then_signal_fence_and_flush();

        match future {
            Ok(future) => {
                future.wait(None).unwrap();
            }
            Err(vulkano::sync::FlushError::OutOfDate) => {
                // Recreate swapchain
            }
            Err(e) => println!("{:?}", e),
        }
    }
}

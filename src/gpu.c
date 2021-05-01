/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gpu.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anystrom <anystrom@hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/30 00:19:59 by anystrom          #+#    #+#             */
/*   Updated: 2021/04/30 00:19:59 by anystrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/wolf.h"
#include "../includes/value.h"

static char		*compile_program(void)
{
	char		*program;
	char		buff[BUFF_SIZE + 1];
	int			fd;
	int			bytes_read;

	fd = open("./src/render.cl", O_RDONLY);
	if (fd == -1)
		return (NULL);
	program = ft_strnew(1);
	while (1)
	{
		ft_bzero(buff, BUFF_SIZE + 1);
		bytes_read = read(fd, buff, BUFF_SIZE + 1);
		buff[bytes_read] = 0;
		program = ft_strfjoin(program, buff);
		if (!bytes_read)
			break;
	}
	return (program);
}

void			*gpu_hint(const char *str)
{
	ft_putendl(str);
	return (NULL);
}

t_gpu			*init_gpu(t_window *win)
{
	t_gpuhandle	handle;
	t_gpu		*gpu;
	int			err;
	char		*source;

	if (!(gpu = (t_gpu*)malloc(sizeof(t_gpu))))
		return (gpu_hint("Not enough memory!"));
	if (clGetPlatformIDs(1, &handle.platform_id, &handle.ret_num_platforms) != CL_SUCCESS)
		return (gpu_hint("Platform not supported!"));
	if (clGetDeviceIDs(handle.platform_id, CL_DEVICE_TYPE_GPU, 1, 
			&gpu->device_id, &handle.ret_num_devices) != CL_SUCCESS)
		return (gpu_hint("ID error!"));
	if (!(gpu->context = clCreateContext(0, 1, &gpu->device_id, NULL, NULL, &err)))
		return (gpu_hint("Context could not be created!"));
	if (!(gpu->commands = clCreateCommandQueue(gpu->context, gpu->device_id, 0, &err)))
		return (gpu_hint("Command queue could not be created!"));
	source = compile_program();
	if (!(gpu->program = clCreateProgramWithSource(gpu->context, 1, (const char**)&source, NULL, &err)))
		return (gpu_hint("GPU program could not be compiled!"));
	free(source);
	err = clBuildProgram(gpu->program, 0, NULL, GPU_FP, NULL, NULL);
	if (err)
	{
		printf("Code: %d: ", err);
		return (gpu_hint("GPU program could not be built!"));
	}
	gpu->kernel = clCreateKernel(gpu->program, "render", &err);
	if (!gpu->kernel || err != CL_SUCCESS)
		return (gpu_hint("Kernel could not be created!"));
	gpu->screen = clCreateBuffer(gpu->context, CL_MEM_READ_WRITE,
		sizeof(Uint32) * win->wid * win->hgt, NULL, NULL);
	gpu->render = clCreateBuffer(gpu->context, CL_MEM_READ_WRITE,
		sizeof(t_render), NULL, NULL);
	gpu->area = clCreateBuffer(gpu->context, CL_MEM_READ_WRITE,
		625, NULL, NULL);
	if (!gpu->screen | !gpu->render | !gpu->area)
		return (gpu_hint("GPU_MEMALLOC issue!"));
	get_gpu_info(gpu, win);
	return (gpu);
}

void	get_gpu_info(t_gpu *gpu, t_window *win)
{
	clGetKernelWorkGroupInfo(gpu->kernel, gpu->device_id, CL_KERNEL_WORK_GROUP_SIZE,
		sizeof(gpu->local), gpu->local, NULL);
	//gpu->global = (win->wid + 31) & ~31;
	gpu->global[0] = 64;
	while (gpu->global[0] < win->wid)
		gpu->global[0] *= 2;
	gpu->global[1] = 64;
	while (gpu->global[1] < win->hgt)
	// && gpu->global[1] < 1024)
		gpu->global[1] *= 2;
	gpu->local[1] = 4;
	printf("Globals: %u %u\nLocals: %u %u\n", gpu->global[0], gpu->global[1], gpu->local[0], gpu->local[1]);
	//exit(0);
}

void	*destroy_gpu(t_gpu *gpu)
{
	clReleaseMemObject(gpu->screen);
    clReleaseMemObject(gpu->render);
    clReleaseProgram(gpu->program);
    clReleaseKernel(gpu->kernel);
    clReleaseCommandQueue(gpu->commands);
    clReleaseContext(gpu->context);
	free(gpu);
	return (NULL);
}

void		copy_map_to_gpu(t_wolf *wlf)
{
	char	map[25][25];
	int		i;

	ft_bzero(map, sizeof(map));
	while (i < wlf->height)
	{
		ft_memcpy(map[i], wlf->area[wlf->flr][i], wlf->width);
		i++;
	}
	printf("%s - %d\n", __FILE__, __LINE__);
	printf("%p\n", wlf->gpu);
	for (int i = 0; i < 25; i++)
		clEnqueueWriteBuffer(wlf->gpu->commands, wlf->gpu->area, CL_TRUE, 25 * i,
			25, map[i], 0, NULL, NULL);
	printf("%s - %d\n", __FILE__, __LINE__);
}

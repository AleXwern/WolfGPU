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

static char		**compile_program(void)
{
	char		**program;
	char		*source;
	char		buff[BUFF_SIZE + 1];
	int			fd;
	int			bytes_read;
	char		sources[SOURCE_COUNT][14] = {
		"render.cl", 
		"draw_floor.cl", 
		"draw_walls.cl",
		"render_vec.cl"};

	program = (char**)malloc(sizeof(char*) * (SOURCE_COUNT + 1));
	for (int i = 0; i < SOURCE_COUNT; i++)
	{
		source = ft_strjoin("./src_gpu/", sources[i]);
		fd = open(source, O_RDONLY);
		free(source);
		if (fd == -1)
			return (NULL);
		program[i] = ft_strnew(1);
		while (1)
		{
			bytes_read = read(fd, buff, BUFF_SIZE + 1);
			buff[bytes_read] = 0;
			program[i] = ft_strfjoin(program[i], buff);
			if (!bytes_read)
				break;
		}
		close(fd);
	}
	program[SOURCE_COUNT] = NULL;
	return (program);
}

static char		*compile_args(void)
{
	char		*arg;

	arg = ft_strjoin(GPU_INCLUDES, GPU_FP);
	arg = ft_strfjoin(arg, GPU_3D);
	printf("Kernel args: %s\n", arg);
	return (arg);
}

static void		*gpu_hint(const char *str)
{
	ft_putendl(str);
	return (NULL);
}

void			test(t_gpu *gpu)	//this is to test saving kernel program binary to disk for later use
{
	size_t		n;
	int			fd;
	uint8_t		**bins;
	size_t		*sizes;
	char		name[] = "./bin/X.out";
	
	clGetProgramInfo(gpu->program, CL_PROGRAM_NUM_DEVICES, 0,NULL, &n);
	sizes = (size_t*)malloc(sizeof(size_t) * n);
	clGetProgramInfo(gpu->program, CL_PROGRAM_BINARY_SIZES, n*sizeof(size_t),sizes, NULL);
	bins = (uint8_t**)malloc(sizeof(uint8_t*) * n);
	for (int i = 0; i < n; i++)
		bins[i] = (uint8_t*)malloc(sizes[i]);
	clGetProgramInfo(gpu->program, CL_PROGRAM_BINARIES, n*sizeof(unsigned char*), bins, NULL);
	for (int i = 0; i < n; i++)
	{
		name[6] = '0' + i;
		fd = open(name, O_WRONLY | O_BINARY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE);
		write(fd, bins[i], sizes[i]);
		close(fd);
		free(bins[i]);
	}
	free(bins);
	free(sizes);
}

t_gpu			*init_gpu(t_window *win)
{
	t_gpuhandle	handle;
	t_gpu		*gpu;
	int			err;
	char		**source;
	char		*args;

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
	if (!(gpu->program = clCreateProgramWithSource(gpu->context, SOURCE_COUNT,
			(const char**)source, NULL, &err)))
	{
		printf("Code: %d: ", err);
		size_t len;
		clGetProgramBuildInfo(gpu->program, gpu->device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &len);
        char *buffer = (char*)malloc(len);
        clGetProgramBuildInfo(gpu->program, gpu->device_id, CL_PROGRAM_BUILD_LOG, len, buffer, NULL);
        printf("%s\n", buffer);
		return (gpu_hint("GPU program could not be compiled!"));
	}
	ft_splitfree(source);
	args = compile_args();
	err = clBuildProgram(gpu->program, 0, NULL, args, NULL, NULL);
	free(args);
	if (err)
	{
		printf("Code: %d: ", err);
		size_t len;
		clGetProgramBuildInfo(gpu->program, gpu->device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &len);
        char *buffer = (char*)malloc(len);
        clGetProgramBuildInfo(gpu->program, gpu->device_id, CL_PROGRAM_BUILD_LOG, len, buffer, NULL);
        printf("%s\n", buffer);
		return (gpu_hint("GPU program could not be built!"));
	}
	gpu->kernel = clCreateKernel(gpu->program, GPU_ENTRY, &err);
	if (!gpu->kernel || err != CL_SUCCESS)
		return (gpu_hint("Kernel could not be created!"));
	gpu->screen = clCreateBuffer(gpu->context, CL_MEM_READ_WRITE,
		sizeof(Uint32) * win->wid * win->hgt, NULL, NULL);
	gpu->render = clCreateBuffer(gpu->context, CL_MEM_READ_WRITE,
		sizeof(t_render), NULL, NULL);
	if (!gpu->screen | !gpu->render)
		return (gpu_hint("GPU_MEMALLOC issue!"));
	get_gpu_info(gpu, win);
	test(gpu);
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
#ifdef	RENDER_VECTOR
	wlf->gpu->area = clCreateBuffer(wlf->gpu->context, CL_MEM_READ_WRITE,
		sizeof(t_map) * wlf->render->vectors, NULL, NULL);
	clEnqueueWriteBuffer(wlf->gpu->commands, wlf->gpu->area, CL_TRUE, 0,
			sizeof(t_map) * wlf->render->vectors, wlf->area, 0, NULL, NULL);
#else
	char	map[MAXHEIGHT][MAXWIDTH];
	int		i;

	wlf->gpu->area = clCreateBuffer(wlf->gpu->context, CL_MEM_READ_WRITE,
		MAXWIDTH * MAXHEIGHT, NULL, NULL);
	ft_bzero(map, sizeof(map));
	while (i < MAXHEIGHT)
	{
		ft_memcpy(map[i], wlf->area[wlf->flr][i], MAXWIDTH);
		i++;
	}
	for (int i = 0; i < 25; i++)
		clEnqueueWriteBuffer(wlf->gpu->commands, wlf->gpu->area, CL_TRUE, MAXHEIGHT * i,
			MAXWIDTH, map[i], 0, NULL, NULL);
#endif
}

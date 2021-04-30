/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gfx.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anystrom <anystrom@hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/06 12:41:51 by anystrom          #+#    #+#             */
/*   Updated: 2021/04/30 23:55:36 by anystrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/wolf.h"
#include "../includes/value.h"

cl_mem		comp_gfx(const char *file, t_gpu *gpu)
{
	int		fd;
	cl_mem	gpu_image;
	t_gfx	image;

	fd = open(file, O_RDONLY | O_BINARY);
	if (fd == -1)
		return (NULL);
	image = read_bmp(file, fd, 0);
	gpu_image = clCreateBuffer(gpu->context, CL_MEM_READ_WRITE,
		sizeof(Uint32) * image.wid * image.hgt, NULL, NULL);
	clEnqueueWriteBuffer(gpu->commands, gpu_image, CL_TRUE, 0,
		sizeof(Uint32) * image.wid * image.hgt, image.data, 0, NULL, NULL);
	return (gpu_image);
}

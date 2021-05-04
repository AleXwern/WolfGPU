/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anystrom <anystrom@hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/09 14:25:29 by anystrom          #+#    #+#             */
/*   Updated: 2021/05/04 15:19:41 by anystrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/wolf.h"
#include "../includes/value.h"

double		pythagor(t_vector f, t_vector s)
{
	t_vector	p;
	double		res;

	p.x = (f.x - s.x) * (f.x - s.x);
	p.y = (f.y - s.y) * (f.y - s.y);
	res = sqrt(p.x + p.y);
	return (res);
}

double			dot_prd(t_vector v, t_vector u)
{
	double	res;

	res = 0;
	res += v.x * u.x;
	res += v.y * u.y;
	return (res);
}

// 0 = pos
// 1 = dir
int		intersect(t_vector *plane, t_vector *ray, t_vector *p)
{
	double	d;
	double	t;

	d = (-plane[1].y * (plane[0].x - ray[0].x) + plane[1].x * (plane[0].y - ray[0].y)) / (-ray[1].x * plane[1].y + plane[1].x * ray[1].y);
	t = (ray[1].x * (plane[0].y - ray[0].y) - ray[1].y * (plane[0].x - ray[0].x)) / (-ray[1].x * plane[1].y + plane[1].x * ray[1].y);
	printf("----\npln: %f,%f - %f,%f\nray: %f,%f - %f,%f\n", plane[0].x, plane[0].y, plane[1].x, plane[1].y, ray[0].x, ray[0].y, ray[1].x, ray[1].y);
	printf("Scalars: %f %f\n", d, t);
	if (/*d >= 0 && d <= 1 && */t >= 0 && t <= 1)
	{
		*p = (t_vector){
			.y = ray[0].y + d * ray[1].y,
			.x = ray[0].x + d * ray[1].x,
			};
		printf("Intersection: %f - %f\n", (*p).x, (*p).y);
		return (1);
	}
	return (0);
}

void		handle_keys(t_wolf *wlf)
{
	if (SDL_PollEvent(&(wlf->event)))
	{
		if (wlf->event.type == SDL_QUIT ||
			wlf->event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
			error_out(FINE, wlf);
		if (wlf->event.key.keysym.scancode < 512)
		{
			if (wlf->event.key.state == SDL_PRESSED)
				wlf->keys[wlf->event.key.keysym.scancode] = 1;
			else if (wlf->event.key.state == SDL_RELEASED)
				wlf->keys[wlf->event.key.keysym.scancode] = 0;
		}
	}
}

void		prep_gpu(t_wolf *wlf, t_gpu *gpu)
{
	int			err;
	t_fpint	olddirx;
	t_fpint	oldplanex;

	/*t_vector	sect;
	t_vector	ray[2] = {wlf->render->pos, wlf->render->dir};
	t_vector	area[2];
	printf("Vecs %u\n", wlf->render->vectors);
	for (size_t i = 0; i < wlf->render->vectors; i++)
	{
		area[0] = wlf->area[i].p1;
		area[1] = wlf->area[i].p2;
		if (intersect(area, ray, &sect))
		{
			printf("Dist: %f\n", pythagor(wlf->render->pos, sect));
		}
	}
	exit(23);*/

	SDL_LockMutex(wlf->mutex);
	err = clEnqueueWriteBuffer(gpu->commands, gpu->render, CL_TRUE, 0, sizeof(t_render), wlf->render, 0, NULL, NULL);
	SDL_UnlockMutex(wlf->mutex);
	err |= clSetKernelArg(gpu->kernel, 0, sizeof(cl_mem), &gpu->screen);
	err |= clSetKernelArg(gpu->kernel, 1, sizeof(cl_mem), &gpu->render);
	err |= clSetKernelArg(gpu->kernel, 2, sizeof(cl_mem), &gpu->area);
	err |= clSetKernelArg(gpu->kernel, 3, sizeof(cl_mem), &gpu->gfx);
	if (err != CL_SUCCESS)
		error_out("Failed to set kernel arguments!", wlf);
	err = clEnqueueNDRangeKernel(gpu->commands, gpu->kernel, 2,
		NULL, gpu->global, gpu->local, 0, NULL, NULL);
	if (err != CL_SUCCESS)
		error_out("Failed to execute kernel!", wlf);
	//SDL_Delay(14);
	handle_keys(wlf);
	clFinish(gpu->commands);
	clEnqueueReadBuffer(gpu->commands, gpu->screen, CL_TRUE, 0, 
		sizeof(Uint32) * wlf->win->wid * wlf->win->hgt, wlf->win->pixels, 0, NULL, NULL);
	SDL_RenderPresent(wlf->win->render);
}

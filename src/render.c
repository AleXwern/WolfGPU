/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anystrom <anystrom@hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/09 14:25:29 by anystrom          #+#    #+#             */
/*   Updated: 2021/05/01 23:31:47 by anystrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/wolf.h"
#include "../includes/value.h"

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
	{
		printf("Code: %d\n", err);
		error_out("Failed to execute kernel!", wlf);
	}
	SDL_Delay(14);
	handle_keys(wlf);
	clFinish(gpu->commands);
	clEnqueueReadBuffer(gpu->commands, gpu->screen, CL_TRUE, 0, 
		sizeof(Uint32) * wlf->win->wid * wlf->win->hgt, wlf->win->pixels, 0, NULL, NULL);
	SDL_RenderPresent(wlf->win->render);
}

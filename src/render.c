/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anystrom <anystrom@hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/09 14:25:29 by anystrom          #+#    #+#             */
/*   Updated: 2021/05/01 20:05:33 by anystrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/wolf.h"
#include "../includes/value.h"

void		move_player(t_wolf *wlf)
{
	t_fpint	olddirx;
	t_fpint	oldplanex;

	if (wlf->keys[SDL_SCANCODE_UP])
	{
		wlf->render->pos.y += wlf->render->dir.y * wlf->movsp;
		wlf->render->pos.x += wlf->render->dir.x * wlf->movsp;
	}
	if (wlf->keys[SDL_SCANCODE_DOWN])
	{
		wlf->render->pos.y -= wlf->render->dir.y * wlf->movsp;
		wlf->render->pos.x -= wlf->render->dir.x * wlf->movsp;
	}
	if (wlf->keys[SDL_SCANCODE_LEFT])
	{
		olddirx = wlf->render->dir.x;
		wlf->render->dir.x = wlf->render->dir.x * cos(-wlf->rotsp) - wlf->render->dir.y * sin(-wlf->rotsp);
		wlf->render->dir.y = olddirx * sin(-wlf->rotsp) + wlf->render->dir.y * cos(-wlf->rotsp);
		oldplanex = wlf->render->plane.x;
		wlf->render->plane.x = wlf->render->plane.x * cos(-wlf->rotsp) - wlf->render->plane.y *
				sin(-wlf->rotsp);
		wlf->render->plane.y = oldplanex * sin(-wlf->rotsp) + wlf->render->plane.y *
				cos(-wlf->rotsp);
	}
	if (wlf->keys[SDL_SCANCODE_RIGHT])
	{
		olddirx = wlf->render->dir.x;
		wlf->render->dir.x = wlf->render->dir.x * cos(wlf->rotsp) - wlf->render->dir.y * sin(wlf->rotsp);
		wlf->render->dir.y = olddirx * sin(wlf->rotsp) + wlf->render->dir.y * cos(wlf->rotsp);
		oldplanex = wlf->render->plane.x;
		wlf->render->plane.x = wlf->render->plane.x * cos(wlf->rotsp) - wlf->render->plane.y *
				sin(wlf->rotsp);
		wlf->render->plane.y = oldplanex * sin(wlf->rotsp) + wlf->render->plane.y *
				cos(wlf->rotsp);
	}
	if (wlf->render->pos.y < 0)
		wlf->render->pos.y = 25;
	else if (wlf->render->pos.y > 25)
		wlf->render->pos.y = 0;
	if (wlf->render->pos.x < 0)
		wlf->render->pos.x = 25;
	else if (wlf->render->pos.x > 25)
		wlf->render->pos.x = 0;
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
	move_player(wlf);
}

void		prep_gpu(t_wolf *wlf, t_gpu *gpu)
{
	int			err;
	t_fpint	olddirx;
	t_fpint	oldplanex;

	err = clEnqueueWriteBuffer(gpu->commands, gpu->render, CL_TRUE, 0, sizeof(t_render), wlf->render, 0, NULL, NULL);
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
	handle_keys(wlf);
	clFinish(gpu->commands);
	clEnqueueReadBuffer(gpu->commands, gpu->screen, CL_TRUE, 0, 
		sizeof(Uint32) * wlf->win->wid * wlf->win->hgt, wlf->win->pixels, 0, NULL, NULL);
	SDL_RenderPresent(wlf->win->render);
}

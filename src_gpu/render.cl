/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.cl                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anystrom <anystrom@hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/02 16:31:14 by anystrom          #+#    #+#             */
/*   Updated: 2021/05/02 16:31:14 by anystrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gpu.h"

void	ray_check(t_raycast *ray, __global char *area)
{
	while (1)
	{
		if (ray->sided.x < ray->sided.y)
		{
			ray->sided.x += ray->delta.x;
			ray->map.x += ray->step.x;
			ray->side = 0;
		}
		else
		{
			ray->sided.y += ray->delta.y;
			ray->map.y += ray->step.y;
			ray->side = 1;
		}
		if (ray->map.y >= 25 || ray->map.x >= 25 ||
			ray->map.y < 0 || ray->map.x < 0)
			break;
		if (area[ray->map.y * 25 + ray->map.x] != 1)
			break;
	}
}

void	ray_prep(t_raycast *ray)
{
	ray->delta.x = 1 / ray->rayd.x;
	ray->delta.y = 1 / ray->rayd.y;
	if (ray->delta.x < 0)
		ray->delta.x *= -1;
	if (ray->delta.y < 0)
		ray->delta.y *= -1;
	if (ray->rayd.x < 0)
	{
		ray->step.x = -1;
		ray->sided.x = (ray->pos.x - ray->map.x) * ray->delta.x;
	}
	else
	{
		ray->step.x = 1;
		ray->sided.x = (ray->map.x + 1.0 - ray->pos.x) * ray->delta.x;
	}
	if (ray->rayd.y < 0)
	{
		ray->step.y = -1;
		ray->sided.y = (ray->pos.y - ray->map.y) * ray->delta.y;
	}
	else
	{
		ray->step.y = 1;
		ray->sided.y = (ray->map.y + 1.0 - ray->pos.y) * ray->delta.y;
	}
}

void	rc_init(t_raycast *ray, __global t_render *render, __global char *area, int x)
{
	ray->camx = 2 * x / (double)(render->maxx) - 1.0;
	ray->rayd.x = render->dir.x + render->plane.x * ray->camx;
	ray->rayd.y = render->dir.y + render->plane.y * ray->camx;
	ray->map.x = (int)ray->pos.x;
	ray->map.y = (int)ray->pos.y;
	ray_prep(ray);
	ray_check(ray, area);
	if (ray->side == 0)
		ray->walldist = (ray->map.x - ray->pos.x + (1 - ray->step.x) / 2) /
				ray->rayd.x;
	else
		ray->walldist = (ray->map.y - ray->pos.y + (1 - ray->step.y) / 2) /
				ray->rayd.y;
}

uint	side_check(t_raycast *ray)
{
	int	delta;

	if (ray->side == 0)
		delta = ray->pos.x - ray->map.x;
	else
		delta = ray->pos.y - ray->map.y;
	if (delta > 0)
		ray->side += 2;
	if (ray->side == 0)
		return (0xff3679ff);
	else if (ray->side == 1)
		return (0xffb01cff);
	else if (ray->side == 2)
		return (0xff1c03fc);
	else
		return (0xff6a3396);
}

void			wall_stripe(
		t_raycast *ray,
		__global t_render *render,
		__global uint *screen,
		__global uint *gfx,
		__global char *area)
{
	t_draw		draw;

	ray->start = ray->y;
	if (render->texbool)
	{
		draw.texnum = area[ray->map.y * 25 + ray->map.x];
		if (ray->side % 2 == 1)
			draw.wallx = render->pos.x + ray->walldist * ray->rayd.x;
		else
			draw.wallx = render->pos.y + ray->walldist * ray->rayd.y;
		draw.wallx -= (int)draw.wallx;
		draw.tex.x = (int)(draw.wallx * 256.0);
		if (ray->side % 2 == 0 && ray->rayd.x > 0)
			draw.tex.x = IMAGEDIM - draw.tex.x - 1;
		else if (ray->side % 2 == 1 && ray->rayd.y < 0)
			draw.tex.x = IMAGEDIM - draw.tex.x - 1;
	}
	//while (ray->start <= ray->end)
	if (1 == 1)
	{
		if (render->texbool)
		{
			draw.tex.y = (((ray->start * DOUBLEDIM - render->maxy * IMAGEDIM +
				ray->lineh * IMAGEDIM) * IMAGEDIM) / ray->lineh) / DOUBLEDIM;
			if (draw.tex.y < 0)
				draw.tex.y *= -1;
			ray->color = gfx[
				draw.tex.y % IMAGEDIM * DOUBLEDIM +
				draw.tex.x % IMAGEDIM
			];
			if (ray->side > 1)
				ray->color = (ray->color >> 1) & 0x7f7f7f;
		}
		screen[render->maxx * ray->start + ray->x] = ray->color;
		ray->start++;
	}
}

void			render_floor(
		t_raycast *ray,
		__global t_render *render,
		__global uint *screen,
		__global uint *gfx,
		__global char *area)
{
	t_draw	draw;

	ray->color = 0xff03B30A;
	if (render->texbool)
	{
		draw.rayd0.x = render->dir.x - render->plane.x;
		draw.rayd0.y = render->dir.y - render->plane.y;
		draw.rayd1.x = render->dir.x + render->plane.x;
		draw.rayd1.y = render->dir.y + render->plane.y;
		draw.rowdist = (render->maxy * 0.5) / (ray->y - render->maxy / 2);
		draw.flstep.x = draw.rowdist * (draw.rayd1.x - draw.rayd0.x) / render->maxx;
		draw.flstep.y = draw.rowdist * (draw.rayd1.y - draw.rayd0.y) / render->maxx;
		draw.floor.x = (render->pos.x + draw.rowdist * draw.rayd0.x) + (draw.flstep.x * ray->x);
		draw.floor.y = (render->pos.y + draw.rowdist * draw.rayd0.y) + (draw.flstep.y * ray->x);
		draw.cell.x = (int)draw.floor.x;
		draw.cell.y = (int)draw.floor.y;
		draw.t.x = (int)(IMAGEDIM * (draw.floor.x - draw.cell.x)) & (IMAGEDIM - 1);
		draw.t.y = (int)(IMAGEDIM * (draw.floor.y - draw.cell.y)) & (IMAGEDIM - 1);
		ray->color = gfx[(DOUBLEDIM * draw.t.y) + (draw.t.x + IMAGEDIM)];
	}
	screen[render->maxx * ray->y + ray->x] = ray->color;
}

__kernel void	render(
		__global uint *screen, 
		__global t_render *render, 
		__global char *area,
		__global uint *gfx)
{
	t_raycast	ray;
	int			x;
	int			y;

	x = get_global_id(0);
	if (x >= render->maxx)
		return;
	y = get_global_id(1);
	if (y >= render->maxy)
		return;
	ray.pos = render->pos;
	rc_init(&ray, render, area, x);
	ray.color = side_check(&ray);
	ray.lineh = (int)(render->maxy / ray.walldist);
	ray.start = -ray.lineh / 2 + render->maxy / 2;
	ray.end = ray.lineh / 2 + render->maxy / 2;
	if (ray.start < 0)
		ray.start = 0;
	if (ray.end >= render->maxy)
		ray.end = render->maxy - 1;
	ray.x = x;
	ray.y = y;
	if (y < ray.start)	//ceiling
		screen[y * render->maxx + x] = 0xff04EEFE;
	else if (y > ray.end)	//floor
		render_floor(&ray, render, screen, gfx, area);
	else
		wall_stripe(&ray, render, screen, gfx, area);
}

__kernel void square(__global float* input, __global float* output, const unsigned int count)
{
	int	i = get_global_id(0);
	char dum[5][5];

	if(i < count)
	{
		output[i] = sizeof(dum);
	}
}

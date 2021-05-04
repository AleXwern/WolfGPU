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
	if (t >= 0 && t <= 1 && d >= 0)
	{
		*p = (t_vector){
			.y = ray[0].y + d * ray[1].y,
			.x = ray[0].x + d * ray[1].x,
			};
		return (1);
	}
	return (0);
}

void	draw(
		__global uint *screen, 
		__global t_render *render, 
		__global t_map *area,
		__global uint *gfx,
		t_raycast *raycast,
		t_vector *anchor)
{
	t_vector	o = {0, 0};
	t_fpint		dist;
	t_fpint		ty;
	uint		texy, texx;

	dist = pythagor(o, *anchor);
	dist -= (int)dist;
	ty = (raycast->y - raycast->start) / ((t_fpint)raycast->end - raycast->start);
	texy = (ty * IMAGEDIM);
	texx = (dist * IMAGEDIM);
	screen[raycast->y * render->maxx + raycast->x] = gfx[texy * DOUBLEDIM + texx];
}

void			draw_floor(
		t_raycast *ray,
		__global t_render *render,
		__global uint *screen,
		__global uint *gfx)
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

__kernel void	render_vectors(
		__global uint *screen, 
		__global t_render *render, 
		__global t_map *area,
		__global uint *gfx)
{
	t_raycast	raycast;
	t_vector	isect;
	t_vector	anchor;
	t_vector	ray[2];
	t_fpint		new_wd;
	t_fpint		angle;
	int			x;
	int			y;

	x = get_global_id(0);
	if (x >= render->maxx)
		return;
	y = get_global_id(1);
	if (y >= render->maxy)
		return;
	//raycast.camx = 2 * x / (double)(render->maxx) - 1.0;
	//raycast.rayd.x = render->dir.x + render->plane.x * raycast.camx;
	//raycast.rayd.y = render->dir.y + render->plane.y * raycast.camx;
	angle = (x - (render->maxx / 2)) / (t_fpint)(render->maxx / 2) * M_PI_4;
	raycast.rayd.x = render->dir.x * cos(angle) - render->dir.y * sin(angle);
	raycast.rayd.y = render->dir.x * sin(angle) + render->dir.y * cos(angle);
	if (y < render->maxy / 2)
		screen[y * render->maxx + x] = 0xff04EEFE;
	else
		screen[y * render->maxx + x] = 0xff03B30A;
	raycast.walldist = FP_MAX;
	ray[0] = render->pos;
	ray[1] = raycast.rayd;
	for (size_t i = 0; i < render->vectors; i++)
	{
		if (intersect(&(area[i]), ray, &isect))
		{
			new_wd = pythagor(render->pos, isect);
			if (new_wd > 0 && new_wd < raycast.walldist)
			{
				raycast.walldist = new_wd;
				anchor = isect;
			}
		}
	}
	raycast.lineh = (int)(render->maxy / raycast.walldist);
	raycast.start = -raycast.lineh / 2 + render->maxy / 2;
	raycast.end = raycast.lineh / 2 + render->maxy / 2;
	raycast.y = y;
	raycast.x = x;
	if (y > raycast.start && y < raycast.end)
		draw(screen, render, area, gfx, &raycast, &anchor);
	else if (y >= raycast.end)
		draw_floor(&raycast, render, screen, gfx);
}

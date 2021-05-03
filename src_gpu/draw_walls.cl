/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw_walls.cl                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anystrom <anystrom@hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/02 16:31:14 by anystrom          #+#    #+#             */
/*   Updated: 2021/05/02 16:31:14 by anystrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gpu.h"

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

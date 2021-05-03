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

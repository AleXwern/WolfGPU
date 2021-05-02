/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gpu.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anystrom <anystrom@hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/02 16:31:14 by anystrom          #+#    #+#             */
/*   Updated: 2021/05/02 16:31:14 by anystrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GPU_H
# define GPU_H

#include "sync_data.h"

#ifndef	HIRES_GFX
# define IMAGEDIM	256
# define DOUBLEDIM	512
# define DOUBLEWID	1024
# define SIZEL		(256 * 8)
#else
# define IMAGEDIM	256
# define DOUBLEDIM	512
# define DOUBLEWID	1024
# define SIZEL		(256 * 8)
#endif

typedef struct		s_raycast
{
	int				lineh;
	t_fpint			walldist;
	int				start;
	int				end;
	int				side;
	int				x;
	int				y;
	unsigned int	color;
	t_vector		pos;
	t_fpint			camx;
	t_vector		rayd;
	t_vector		delta;
	t_vector		sided;
	t_ivector		map;
	t_ivector		step;
}					t_raycast;

typedef struct		s_draw
{
	int				texnum;
	t_fpint			wallx;
	t_fpint			pos;
	t_fpint			posz;
	t_fpint			rowdist;
	t_ivector		tex;
	t_vector		rayd0;
	t_vector		rayd1;
	t_vector		flstep;
	t_vector		floor;
	t_ivector		cell;
	t_ivector		t;
}					t_draw;

#endif

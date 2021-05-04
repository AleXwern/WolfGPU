/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sync_data.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anystrom <anystrom@hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/02 16:34:10 by anystrom          #+#    #+#             */
/*   Updated: 2021/05/04 00:26:33 by anystrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SYNC_DATA_H
# define SYNC_DATA_H

/*
**	This is to make sure that host and kernel programs are synced and shared
**	information (structs and defines) are exactly the same on both sides.
**	EX. if I decide that all XYZ dimensions are used then it's automatically
**	enabled for host and kernel without any need for further modifications.
*/

/*
**	Floating point precicion. Also affects if ivector is 64-bit wide.
*/
# if	SINGLE_PRECICION == 1
typedef	float			t_fpint;
typedef int				t_int;
#  define FP_MAX		3.402823466e+38F
#  define GPU_FP		"-DSINGLE_PRECICION=1 "
# else
typedef	double			t_fpint;
typedef long			t_int;
#  define FP_MAX		1.7976931348623158e+308
#  define GPU_FP		"-DSINGLE_PRECICION=0 "
# endif

/*
**	3D-ness. If activated will take Z dimension into account and renders true 3D world.
**	Even without it, the program simulates 3D-ness but is only limited to 2 dimesions and
**	gives and illusion of 3D world.
**	Could also be used to limit how much GPU is used for rendering if disabled.
*/
# if	DOOM_RENDER == 1
#  define IS3D
#  define GPU_3D		"-DDOOM_RENDER=1 "
# else
#  define GPU_3D		"-DDOOM_RENDER=0 "
# endif

typedef struct			s_vector
{
	t_fpint				x;
	t_fpint				y;
# ifdef			IS3D
	t_fpint				z;
# endif
}						t_vector;

typedef struct			s_ivector
{
	t_int				x;
	t_int				y;
# ifdef			IS3D
	t_int				z;
# endif
}						t_ivector;

typedef struct			s_map
{
	t_vector			p1;
	t_vector			p2;
}						t_map;


typedef struct			s_render
{
	int					maxx;
	int					maxy;
	unsigned int		vectors;
	t_vector			pos;
	t_vector			dir;
	t_vector			plane;
	char				texbool;
}						t_render;

#endif

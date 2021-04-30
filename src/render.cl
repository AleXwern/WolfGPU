
//	***************STRUCTS*****************
#if SINGLE_PRECICION == 1
typedef	float		t_fpint;
#else
typedef	double		t_fpint;
#endif

typedef struct		s_vector
{
	t_fpint			x;
	t_fpint			y;
#ifdef			IS3D
	t_fpint			z;
#endif
}					t_vector;

typedef struct		s_ivector
{
	int				x;
	int				y;
#ifdef			IS3D
	int				z;
#endif
}					t_ivector;

typedef struct		s_render
{
	int				maxx;
	int				maxy;
	t_vector		pos;
	t_vector		dir;
	t_vector		plane;
	char			texbool;
}					t_render;

typedef struct		s_raycast
{
	int				lineh;
	t_fpint			walldist;
	int				start;
	int				end;
	int				side;
	uint			color;
	t_vector		pos;
	t_fpint			camx;
	t_vector		rayd;
	t_vector		delta;
	t_vector		sided;
	t_ivector		map;
	t_ivector		step;
}					t_raycast;

//  ****************FUNCTIONS******************

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
		if (area[ray->map.y * 25 + ray->map.x] != 1)
			break;
		if (ray->map.y >= 25 || ray->map.x >= 25 ||
			ray->map.y < 0 || ray->map.x < 0)
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
	if (ray->walldist < 0.0001)
		ray->walldist += 0.01;
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

void			wall_stripe(t_raycast *ray, __global t_render *render, __global uint *screen, int x)
{
	while (ray->start <= ray->end)
	{
		screen[render->maxx * ray->start + x] = ray->color;
		ray->start++;
	}
}

__kernel void	render(__global uint *screen, __global t_render *render, __global char *area)
{
	t_raycast	ray;
	int			x;

	x = get_global_id(0);
	if (x >= render->maxx)
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
	for (int i = 0; i < ray.start; i++)	//ceiling
		screen[i * render->maxx + x] = 0xff04EEFE;
	wall_stripe(&ray, render, screen, x);
	for (int i = ray.end; i < render->maxy; i++)	//ceiling
		screen[i * render->maxx + x] = 0xff03B30A;
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
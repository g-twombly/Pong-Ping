#ifndef __FORCES_H__
#define __FORCES_H__

#include "scene.h"

/**
 * A function called when a collision occurs.
 * @param body1 the first body passed to create_collision()
 * @param body2 the second body passed to create_collision()
 * @param axis a unit vector pointing from body1 towards body2
 *   that defines the direction the two bodies are colliding in
 * @param aux the auxiliary value passed to create_collision()
 */
typedef void (*collision_handler_t)(body_t *body1, body_t *body2, vector_t axis,
                                    void *aux);

/**
 * A force is made up of a force_creator_t (forcer), auxiliary values,
 * and a free function.
 */
typedef struct force force_t;

/**
 * Contains elasticity value (coefficient of restitution) for collisions.
 */
typedef struct elast elast_t;

/**
 * Contains auxiliary values for two body interactions or for one body.
 */
typedef struct aux aux_t;

/**
 * Contains a boolean for collisions.
 */
typedef struct collision_aux collision_aux_t;

/**
 * Releases the memory allocated for an aux struct with multiple values.
 *
 * @param aux a pointer to a struct for multiple bodies.
 */
void aux_free(aux_t *aux);

/**
 * Releases the memory allocated for an aux with multiple values.
 *
 * @param aux a pointer to a struct for collision_aux_t aux containing a
 * handler, freer, and boolean.
 */
void collision_aux_free(collision_aux_t *aux);

/**
 * Allocates memory for an aux with the given parameters (2 bodies).
 *
 * @param constant a constant value from equation
 * @param body1 pointer to the first body
 * @param body2 pointer to the second body
 * @return a pointer to a newly allocated aux
 */
aux_t *aux_init_two_bodies(double constant, body_t *body1, body_t *body2);

/**
 * Allocates memory for an aux with the given parameters (one body).
 *
 * @param constant a constant value from equation
 * @param body pointer to the only body
 * @return a pointer to a newly allocated aux
 */
aux_t *aux_init_one_body(double constant, body_t *body);

/**
 * Allocates memory for bodies in collisions.
 * Asserts that the required memory is successfully allocated.
 *
 * @return the collision bodies
 */
collision_aux_t *aux_init_collision();

/**
 * Allocates memory for a force.
 * Asserts that the required memory is successfully allocated.
 *
 * @return the new force
 */
force_t *force_init(force_creator_t force_c, void *aux, free_func_t freer,
                    list_t *bodies);

/**
 * Frees the memory allocated for a force, including its aux value.
 *
 * @param force a pointer to the force to be freed
 */
void force_free(force_t *force);

/**
 * Returns the list of associated bodies.
 *
 * @param force the force
 * @return a pointer to the list of bodies
 */
list_t *force_get_bodies(force_t *force);

/**
 * Returns the body at a specific index of the bodies list.
 * Asserts that the index is valid.
 *
 * @param force the force
 * @param index the index
 * @return a pointer to the body
 */
list_t *force_get_body(force_t *force, size_t index);

/**
 * Returns whether the bodies list contains a particular body.
 *
 * @param force the force
 * @param body the body
 * @return whether the bodies list contains body
 */
bool force_is_body_in_force(force_t *force, body_t *body);

/**
 * Applies a force creator to its aux values.
 *
 * @param force a force as returned from force_init
 */
void apply_force_creator(force_t *force);

/**
 * A function which computes newtonian gravity force for the body.
 * Takes in an auxiliary value that can store parameters or state (2 bodies).
 */
void newtonian_force(aux_t *aux_val);

/**
 * A function which computes spring force for the body.
 * Takes in an auxiliary value that can store parameters or state (2 bodies).
 */
void spring_force(aux_t *aux_val);

/**
 * A function which computes drag force for the body.
 * Takes in an auxiliary value that can store parameters or state (only 1 body).
 */
void drag_force(aux_t *aux_val);

/**
 * Adds a force creator to a scene that applies gravity between two bodies.
 * The force creator will be called each tick
 * to compute the Newtonian gravitational force between the bodies.
 * See
 * https://en.wikipedia.org/wiki/Newton%27s_law_of_universal_gravitation#Vector_form.
 * The force should not be applied when the bodies are very close,
 * because its magnitude blows up as the distance between the bodies goes to 0.
 *
 * @param scene the scene containing the bodies
 * @param G the gravitational proportionality constant
 * @param body1 the first body
 * @param body2 the second body
 */
void create_newtonian_gravity(scene_t *scene, double G, body_t *body1,
                              body_t *body2);

/**
 * Adds a force creator to a scene that acts like a spring between two bodies.
 * The force creator will be called each tick
 * to compute the Hooke's-Law spring force between the bodies.
 * See https://en.wikipedia.org/wiki/Hooke%27s_law.
 *
 * @param scene the scene containing the bodies
 * @param k the Hooke's constant for the spring
 * @param body1 the first body
 * @param body2 the second body
 */
void create_spring(scene_t *scene, double k, body_t *body1, body_t *body2);

/**
 * Adds a force creator to a scene that applies a drag force on a body.
 * The force creator will be called each tick
 * to compute the drag force on the body proportional to its velocity.
 * The force points opposite the body's velocity.
 *
 * @param scene the scene containing the bodies
 * @param gamma the proportionality constant between force and velocity
 *   (higher gamma means more drag)
 * @param body the body to slow down
 */
void create_drag(scene_t *scene, double gamma, body_t *body);

/**
 * Adds a force creator to a scene that calls a given collision handler
 * function each time two bodies collide.
 * This generalizes create_destructive_collision() from last week,
 * allowing different things to happen on a collision.
 * The handler is passed the bodies, the collision axis, and an auxiliary value.
 * It should only be called once while the bodies are still colliding.
 *
 * @param scene the scene containing the bodies
 * @param body1 the first body
 * @param body2 the second body
 * @param handler a function to call whenever the bodies collide
 * @param aux an auxiliary value to pass to the handler
 * @param freer if non-NULL, a function to call in order to free aux
 */
void create_collision(scene_t *scene, body_t *body1, body_t *body2,
                      collision_handler_t handler, void *aux,
                      free_func_t freer);

/**
 * Adds a force creator to a scene that destroys two bodies when they collide.
 * The bodies should be destroyed by calling body_remove().
 * This should be represented as an on-collision callback
 * registered with create_collision().
 *
 * @param scene the scene containing the bodies
 * @param body1 the first body
 * @param body2 the second body
 */
void create_destructive_collision(scene_t *scene, body_t *body1, body_t *body2);

/**
 * Adds a force creator to a scene that applies impulses
 * to resolve collisions between two bodies in the scene.
 * This should be represented as an on-collision callback
 * registered with create_collision().
 *
 * You may remember from project01 that you should avoid applying impulses
 * multiple times while the bodies are still colliding.
 * You should also have a special case that allows either body1 or body2
 * to have mass INFINITY, as this is useful for simulating walls.
 *
 * @param scene the scene containing the bodies
 * @param elasticity the "coefficient of restitution" of the collision;
 * 0 is a perfectly inelastic collision and 1 is a perfectly elastic collision
 * @param body1 the first body
 * @param body2 the second body
 */
void create_physics_collision(scene_t *scene, double elasticity, body_t *body1,
                              body_t *body2);

/**
 * Applies collision force on the bodies.
 *
 * @param other_aux aux information
 */
void collision_force_creator(collision_aux_t *other_aux);

#endif // #ifndef __FORCES_H__

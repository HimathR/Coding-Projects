import {
  increment_properties,
  set_properties,
  get_properties,
} from "./update_properties.js"


// Elements
const dino_element = document.querySelector("[data-dino]")
const jump_velocity = 0.45
const gravity = 0.0015

let jumping
let dino_animation
let frame_time_current
let vertical_velocity


export function prepare_dino() {
  jumping = false
  dino_animation = 0
  frame_time_current = 0
  vertical_velocity = 0
  set_properties(dino_element, "--bottom", 0)
  document.removeEventListener("keydown", when_jumping)
  document.addEventListener("keydown", when_jumping)
}

export function update_dino(delta, speedScale) {
  make_dino_run(delta, speedScale)
  make_dino_jump(delta)
}

export function get_dino_hitbox() {
  return dino_element.getBoundingClientRect()
}

function make_dino_run(delta, speedScale) {
  if (jumping) {
    dino_element.src = `dinoimgs/dino-lose.png`
    return
  }
  if (frame_time_current >= 100) {
    dino_animation = (dino_animation + 1) % 2 // dino frame is 0 or 1
    dino_element.src = `dinoimgs/dino-run-${dino_animation}.png`
    frame_time_current -= 100 // reset frame time
  }
  frame_time_current += delta * speedScale
}

function make_dino_jump(delta) {
  if (!jumping) return

  increment_properties(dino_element, "--bottom", vertical_velocity * delta) // move dino up based on velocity

  if (get_properties(dino_element, "--bottom") <= 0) { // once dino hits the floor again, stop jumping
    set_properties(dino_element, "--bottom", 0)
    jumping = false
  }

  vertical_velocity -= gravity * delta // apply gravity by slowing down vertical velocity
}

function when_jumping(e) {
  if (e.code !== "Space" || jumping) return 
  // make sure we can't keep jumping more when we're already jumping

  vertical_velocity = jump_velocity
  jumping = true
}

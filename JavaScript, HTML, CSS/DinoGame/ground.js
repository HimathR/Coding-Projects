import {
  get_properties,
  increment_properties,
  set_properties,
} from "./update_properties.js"

const dino_speed = 0.05
const ground_elements = document.querySelectorAll("[data-ground]")

export function setup_ground() {
  set_properties(ground_elements[0], "--left", 0) // ground elements is called twice in the html file, 
  set_properties(ground_elements[1], "--left", 300)  // so we need to set the left property twice
}

export function update_ground(delta, speedScale) {
  ground_elements.forEach(ground => {
    increment_properties(ground, "--left", delta * speedScale * dino_speed * -1)

    if (get_properties(ground, "--left") <= -300) { // when the ground is off the screen,
      increment_properties(ground, "--left", 600) // move the ground back around
    }
  })
}
 
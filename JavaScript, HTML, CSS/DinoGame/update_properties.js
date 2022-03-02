
export function get_properties(elem, prop) {
  return parseFloat(getComputedStyle(elem).getPropertyValue(prop)) || 0
}

export function set_properties(elem, prop, value) {
  elem.style.setProperty(prop, value)
}

// main function, which is called every frame to move the various moving elements on the screen
export function increment_properties(elem, prop, inc) {
  set_properties(elem, prop, get_properties(elem, prop) + inc)
}

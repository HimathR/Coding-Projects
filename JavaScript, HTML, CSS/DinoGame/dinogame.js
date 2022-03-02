import { update_ground, setup_ground } from "./ground.js"
import { update_dino as update_dino, prepare_dino as setup_dino, get_dino_hitbox} from "./dino.js"
import { update_obstacles, generate_obstacles, get_obstacle_hitboxes } from "./obstacles.js"

// Parameters
const width = 100
const height = 30
const speed_increase = 0.00001

// Board Elements
const world_elements = document.querySelector("[data-world]")
const startscreen_elements = document.querySelector("[data-start-screen]")

// Event Listeners
scale_world()
window.addEventListener("resize", scale_world)
document.addEventListener("keydown", start_game, { once: true })

// Initialize Key Variables
let last_time
let speed_scale
let score 

function scale_world() { 
  let scale
  if (window.innerWidth / window.innerHeight < width / height) { // if the window is wider than it is tall
    scale = window.innerWidth / width // scale the world to fit the width of the window
  } else {
    scale = window.innerHeight / height // otherwise scale the world to fit the height of the window
  }

  // apply the scale so that the dino game fits the window properly
  world_elements.style.width = `${width * scale}px` 
  world_elements.style.height = `${height * scale}px`
}

function start_game() {
  last_time = null // making sure everything starts from scratch
  speed_scale = 1
  score = 0
  // setup the world field
  setup_ground() 
  setup_dino()
  generate_obstacles()
  // transitions for the game
  startscreen_elements.classList.toggle("peace-out")
  window.requestAnimationFrame(update)
}


function update(time) {
  if (last_time == null) {
    last_time = time
    window.requestAnimationFrame(update)  
    return
  }
  const delta = time - last_time

  // update world field
  update_ground(delta, speed_scale)
  update_dino(delta, speed_scale)
  update_obstacles(delta, speed_scale)
  update_speed(delta)
  update_score(delta)

  if (check_lose()) return handle_lose()
  last_time = time
  window.requestAnimationFrame(update)
}


function collision_detected(hitbox1, hitbox2) { // check if two hitboxes are colliding
  return (
    hitbox1.left < hitbox2.right &&
    hitbox1.top < hitbox2.bottom &&
    hitbox1.right > hitbox2.left &&
    hitbox1.bottom > hitbox2.top
  )
}

function update_speed(delta) {
  speed_scale += delta * speed_increase // update the speed the ground moves
}

function update_score(delta) {
  score += delta * 0.01 // every second we get 10 points
  document.querySelector("[data-score]").textContent = Math.floor(score) // update text contents of score
}

function check_lose() {
  const dino_hitbox = get_dino_hitbox()
  return get_obstacle_hitboxes().some(hitbox => collision_detected(hitbox, dino_hitbox)) // check if dino has hit an obstacle
}

function handle_lose() {
  document.querySelector("[data-dino]").src = 'dinoimgs/dino-lose.png' // give the lose face to dino
  check_score() // check if the current score is higher than the highest score
  setTimeout(() => { 
    document.addEventListener("keydown", start_game, { once: true })
    startscreen_elements.classList.toggle("peace-out")
  }, 100) 
}

function check_score(){
  var str = document.querySelector("[data-hiscore]").textContent // get the current highest score in this session
  var hiscoreres = str.replace(/\D/g, "");  // replace any non-digit characters with nothing
  if (score > hiscoreres) { 
    document.querySelector("[data-hiscore]").textContent = "Highest Score: " + Math.floor(score) // update the highest score if the current score is higher
  }
}



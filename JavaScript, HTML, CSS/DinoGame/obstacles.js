import {
  set_properties,
  increment_properties,
  get_properties,
} from "./update_properties.js";

// Game Parameters
const dino_speed = 0.05; // how fast dino runs
// minimum and maximum amount of time before the next obstacle appears
const earliest_obst_appearance = 800;
const latest_obst_appearance = 2500;
// the world element is the parent element of all obstacles
const world_element = document.querySelector("[data-world]");
let next_obstacle;

export function generate_obstacles() {
  next_obstacle = earliest_obst_appearance;
  document.querySelectorAll("[data-obstacle]").forEach((obstacle) => {
    obstacle.remove();
  });
}

export function update_obstacles(delta, speedScale) {
  // move cobstacle to the left based on parameters and current speed of the board
  document.querySelectorAll("[data-obstacle]").forEach((obstacle) => {
    increment_properties(obstacle, "--left", delta * speedScale * dino_speed * -1); 
    // when the obstacle is off the screen, remove it
    if (get_properties(obstacle, "--left") <= -100) {
      obstacle.remove(); 
    }
  });

  // if the next obstacle is ready, create it
  if (next_obstacle <= 0) {
    // generate 4 random numbers: this will determine the unique obstacle the board will show
    let obsttype = Math.floor(Math.random() * 4);
    let theme = check_id(); // check which button is disabled
    create_obstacles(obsttype, theme); 
    next_obstacle =
      get_randnum(
        earliest_obst_appearance,
        latest_obst_appearance
      ) / speedScale; // determine when the next obstacle will appear
  }
  next_obstacle -= delta;
}

// the hitboxes of the obstacles are rectangular in shape, so we can use the 
// same boundingclientrect function to check if the dino hit the obstacle
export function get_obstacle_hitboxes() {
  return [...document.querySelectorAll("[data-obstacle]")].map((obstacle) => {
    return obstacle.getBoundingClientRect();
  });
}

function check_id() { // returns the id of the current button disabled: this tells us the current theme
  for (var i = 1; i <= 3; i++) {
    let id = "b" + i;
    console.log(id, "is disabled?", document.getElementById(id).disabled);
    if (document.getElementById(id).disabled == true) {
      return id;
    }
  }
}

function create_obstacles(obsttype, theme) {
  const obstacle = document.createElement("img"); 
  obstacle.setAttribute("data-obstacle", "");
  obstacle.dataset.obstacle = true;
  console.log(theme);
  if (theme == "b1") { // default theme
    if (obsttype == 1) {
      obstacle.src = "dinoimgs/cactus1.png";
    } else if (obsttype == 2) {
      obstacle.src = "dinoimgs/cactus2.png";
    } else if (obsttype == 3) {
      obstacle.src = "dinoimgs/cactus3.png";
    } else {
      obstacle.src = "dinoimgs/cactus.png";
    }
  } else if (theme == "b2") { //ocean theme
    if (obsttype == 1) {
      obstacle.src = "dinoimgs/pufferfish.gif";
    } else if (obsttype == 2) {
      obstacle.src = "dinoimgs/shark.png";
    } else if (obsttype == 3) {
      obstacle.src = "dinoimgs/mudkip.gif";
    } else {
      obstacle.src = "dinoimgs/corals.gif";
    }
  } else { // space theme
    if (obsttype == 1) {
      obstacle.src = "dinoimgs/ufo2.png";
    } else if (obsttype == 2) {
      obstacle.src = "dinoimgs/moon.gif";
    } else if (obsttype == 3) {
      obstacle.src = "dinoimgs/sun.gif";
    } else {
      obstacle.src = "dinoimgs/alien.gif";
    }
  }

  // set the obstacle's position with the CSS properties
  obstacle.classList.add("obstacle");
  set_properties(obstacle, "--left", 100);
  world_element.append(obstacle);
}

function get_randnum(min, max) {
  return Math.floor(Math.random() * (max - min + 1) + min);
}

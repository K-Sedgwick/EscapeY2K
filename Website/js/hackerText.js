const letters="ABCDEFGHIJKLMNOPQRSTUVWXYZ";

document.getElementById('hacker-text').onmouseover = event => {
  let iteration = 0;
  let word = event.target.innerText;
  
  const interval = setInterval(() => {
    event.target.innerText = word.split("")
    .map((letter, index) => {
      if(index < iteration){
        return event.target.dataset.word[index];
      }
      return letters[Math.floor(Math.random() * 26)];
    })
    .join("");
    
    if(iteration > word.length){
      clearInterval(interval);
    }
    
    iteration += 1;
  }, 30); 
}

function randomRange(min, max){
  return Math.random() * (max - min) + min;
}
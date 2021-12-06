/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-mpu-6050-web-server/

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/

let scene, camera, rendered, cube;

function parentWidth(elem) {
  return elem.parentElement.clientWidth;
}

function parentHeight(elem) {
  return elem.parentElement.clientHeight;
}

function init3D(){
  scene = new THREE.Scene();
  scene.background = new THREE.Color(0x000000);

  camera = new THREE.PerspectiveCamera(75, parentWidth(document.getElementById("3Dcube")) / parentHeight(document.getElementById("3Dcube")), 0.1, 1000);

  renderer = new THREE.WebGLRenderer({ antialias: true });
  renderer.setSize(parentWidth(document.getElementById("3Dcube")), parentHeight(document.getElementById("3Dcube")));

  document.getElementById('3Dcube').appendChild(renderer.domElement);

  // Create a geometry
  const geometry = new THREE.BoxGeometry(5, 1, 4);

  // Materials of each face
  var cubeMaterials = [
    new THREE.MeshBasicMaterial({color:0x008080}),
    new THREE.MeshBasicMaterial({color:0x008080}),
    new THREE.MeshBasicMaterial({color:0x008080}),
    new THREE.MeshBasicMaterial({color:0x008080}),
    new THREE.MeshBasicMaterial({color:0x008080}),
    new THREE.MeshBasicMaterial({color:0x008080}),
  ];

  const material = new THREE.MeshFaceMaterial(cubeMaterials);

  cube = new THREE.Mesh(geometry, material);
  scene.add(cube);
  camera.position.z = 5;
  renderer.render(scene, camera);
}

// Resize the 3D object when the browser window changes size
function onWindowResize(){
  camera.aspect = parentWidth(document.getElementById("3Dcube")) / parentHeight(document.getElementById("3Dcube"));
  //camera.aspect = window.innerWidth /  window.innerHeight;
  camera.updateProjectionMatrix();
  //renderer.setSize(window.innerWidth, window.innerHeight);
  renderer.setSize(parentWidth(document.getElementById("3Dcube")), parentHeight(document.getElementById("3Dcube")));

}

window.addEventListener('resize', onWindowResize, false);

// Create the 3D representation
//init3D();

// Create events for the sensor readings //在这里追加上传数据的时间，接收时间
if (!!window.EventSource) {
  var source = new EventSource('/events');

  source.addEventListener('open', function(e) {
    console.log("Events Connected");
  }, false);

  source.addEventListener('error', function(e) {
    if (e.target.readyState != EventSource.OPEN) {
      console.log("Events Disconnected");
    }
  }, false);

  source.addEventListener('temperature_reading', function(e) {
    console.log("temperature_reading", e.data);
    document.getElementById("temp").innerHTML = e.data;
  }, false);
  
  //追加变量参数时间，我用来测试ESP32上传的是数据
  source.addEventListener('temptag', function(e) {
    console.log("temptag", e.data);
    document.getElementById("temptag").innerHTML = e.data;
  }, false);
  
  source.addEventListener('temp', function(e) {
    console.log("temp", e.data);
    document.getElementById("temp").innerHTML = e.data;
  }, false);
  source.addEventListener('systemstate', function(e) {
    console.log("systemstate", e.data);
    document.getElementById("systemstate").innerHTML = e.data;
  }, false);
}

function sendid(element){
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/"+element.id, true);
  console.log(element.id);
  xhr.send();
}
function setting(){
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/setvalue?message="+document.getElementById("setvalue").value, true);
  // console.log(element.id);
  xhr.send();
}

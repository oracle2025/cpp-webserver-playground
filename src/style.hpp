#pragma once

constexpr auto STYLE_SHEET = R"(body {
font-family: Arial, sans-serif;
background-color: #f4f4f4;
margin: 0px;
}
.container {
background-color: #fff;
padding: 20px 0px;
max-width: 500px;
margin: auto;
margin-top: 50px;
box-shadow: 0px 0px 10px rgba(0,0,0,0.2);
}
span.label {
display: none;
}
h2 {
text-align: center;
margin-bottom: 20px;
}
input[type=text], input[type=password] {
width: 100%;
padding: 12px 20px;
margin: 8px 0;
display: inline-block;
border: 1px solid #ccc;
border-radius: 4px;
box-sizing: border-box;
}
button[type=submit] {
background-color: #4CAF50;
color: white;
padding: 14px 20px;
margin: 8px 0;
border: none;
border-radius: 4px;
cursor: pointer;
width: 100%;
}
button[type=submit]:hover {
background-color: #45a049;
}
table {
border-collapse: collapse;
width: 100%;
table-layout: auto;
}
tr {
border-top: 1px solid gray;
border-bottom: 1px solid gray;
}
tr.uneven {
background-color: #eee;
}
.edit {
background-color: #007bff;
}
.remove {
background-color: #dc3545;
}
.create {
background-color: #28a745;
}
.button {
border: none;
color: white;
text-align: center;
text-decoration: none;
display: inline-block;
cursor: pointer;
padding: 1px 10px;
margin: 2px 0px;
height: 24px;
border-radius: 4px;
}
:where(th, td):not(.max) {
  width: 0;
  white-space: nowrap;
}
@media (min-width:540px)  {
.container {
        background-color: #fff;
        padding: 20px;
        max-width: 500px;
        margin: auto;
        margin-top: 50px;
        box-shadow: 0px 0px 10px rgba(0,0,0,0.2);
}
span.label {
        display: inline;
}
}
)";
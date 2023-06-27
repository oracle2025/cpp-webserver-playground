#pragma once

constexpr auto STYLE_SHEET = R"(body {
font-family: Arial, sans-serif;
margin: 0px;
  background-color: #fefefe;
}

.header {
    margin-top: 0px;
    padding-top: 0px;
    padding-bottom: 0px;
    background-color: rgb(33, 37, 41);
    color: white;
}
.header div {
    display: block;
    padding: 10px;
}
.header a {
    color: rgba(255, 255, 255, 0.75);
    text-decoration: none;
    display: block;
    padding: 10px;
}
.header ul {
    display: float;
    margin: 0px;
    padding: 0px;
    float: none;
    vertical-align: top;
    width: 100%;
    overflow: hidden;
}
.header ul li {
    margin: 0px;
    padding: 0px;
    list-style: none;
    display: block;
    float: left;
    vertical-align: center;
}
.header ul li.right {
    float: right;
}

span.label {
display: none;
}
h2 {
text-align: center;
margin-bottom: 20px;
}
input[type=text], input[type=password], select {
width: 100%;
padding: 12px 20px;
margin: 8px 0;
display: inline-block;
border: 1px solid #ccc;
border-radius: 4px;
box-sizing: border-box;
}
button[type=submit] {
padding: 14px 20px;
margin: 8px 0;
border: none;
border-radius: 4px;
cursor: pointer;
width: 100%;
}
button[type=submit].btn-success {
    background-color: #4CAF50;
    color: white;
}
button[type=submit].btn-danger {
    color: #fff;
    background-color: #dc3545;
}
button[type=submit].btn-light {
    color: #212529;
    background-color: #f8f9fa;
}
button[type=submit]:hover.btn-success {
background-color: #45a049;
}
button[type=submit]:hover.btn-danger {
background-color: #c82333;
}
button[type=submit]:hover.btn-light {
background-color: #e2e6ea;
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
.container {
padding: 20px 5px;
max-width: 500px;
margin: auto;
margin-top: 0px;
}
@media (min-width:540px)  {
.container {
        padding: 20px;
        max-width: 500px;
        margin: auto;
        margin-top: 0px;
    box-shadow: 0px 0px 10px rgba(0,0,0,0.2);
        background-color: #fefefe;
}
    body {
    background-color: #f4f4f4;
    }
span.label {
        display: inline;
}
}
.alert-danger, .alert-success, .alert-primary, .alert-secondary, .alert-warning, .alert-info {
    padding: 14px;
    margin-bottom: 10px;
}
.alert-danger {
    color: #721c24;
    background-color: #f8d7da;
    border: 1px solid #f5c6cb;
}
.alert-success {
    color: #155724;
    background-color: #d4edda;
    border: 1px solid #c3e6cb;
}
.alert-primary {
    color: #004085;
    background-color: #cce5ff;
    border: 1px solid #b8daff;
}
.alert-secondary {
    color: #383d41;
    background-color: #e2e3e5;
    border: 1px solid #d6d8db;
}
.alert-warning {
    color: #856404;
    background-color: #fff3cd;
    border: 1px solid #ffeeba;
}
.alert-info {
    color: #0c5460;
    background-color: #d1ecf1;
    border: 1px solid #bee5eb;
}
)";
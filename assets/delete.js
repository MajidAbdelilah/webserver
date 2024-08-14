var url = "http://127.0.0.1:8080/delete_test.txt";
var xhr = new XMLHttpRequest();

xhr.open("DELETE", url, true);
xhr.onload = function () {
    var users = JSON.parse(xhr.responseText);
    if (xhr.readyState == 4 && xhr.status == "200") {
        console.table(users);
    } else {
        console.error(users);
    }
}
xhr.send(null);
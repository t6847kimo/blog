---
title: "用FB打卡紀錄在Google Map打點"
layout: post
tags: [Google Map, Facebook, Web]
published: false
comments: false
---
這一篇我們要用上一篇取得的資料來打點了!

## Google MAP API Overview

先說個不幸的消息, Google MAP API從2018年七月開始需要收費, 引述[Google地圖改變收費模式，API計費變嚴變貴影響政府機構](https://www.bnext.com.tw/article/49903/google-maps-api-charge)的說明

> 今年5月2日Google發文預告，將在7月16日開發，調整收費模式。每位用到Google地圖架設服務的開發者，不論是否達到Google地圖收費的標準，都得申請API key，並且綁訂信用卡卡號。而每位都有每位200美元的免費額度，超過才會需要繳錢給Google。

不過我們不用付費還是可以用最基本的功能來Demo, 只是會跳出警示跟浮水印, 如果真的要當產品供人大量使用的就要三思了! 有顧慮的話可以考慮開源免費的OpenStreet Map , 圖資主要以國外的較充足且API可能相較起來沒這麼強大, 沒辦法, 畢竟一分錢一分貨嘛!

Google Map的


### Full Code

在下面按下**取得代碼 -> Javascript SDK**我們即可取得Query的Code, 但是必須先初始化你的App以及提供access_token, 記得替換掉以下`YOUR_APP_ID` & `YOUR_ACCESS_TOKEN`
* YOUR_APP_ID 可由**我的應用程式 ->應用程式編號**取得
* YOUR_ACCESS_TOKEN 可由**圖型API測式工具 -> 存取權杖**取得


```html
<!DOCTYPE html>
<html>
  <head>
    <meta http-equiv="content-type" content="text/html; charset=utf-8" />
    <script async defer src="https://connect.facebook.net/en_US/sdk.js"></script>
    <script>
    function initFbData(){
      FB.init({
        appId            : 'YOUR_APP_ID',
        autoLogAppEvents : true,
        xfbml            : true,
        version          : 'v3.2'
      });
      FB.api(
        '/me',
        'GET',
        {
          "fields":"id,name,posts{place,message}",
          "access_token": "YOUR_ACCESS_TOKEN"
        },
        function(response) {
          document.getElementById("content").innerHTML+= JSON.stringify(response);
        }
      );
    }
    </script>
  </head>
  <body onload="initFbData();">
    <div id ="content"></div>
  </body>
</html>
```
<!--stackedit_data:
eyJoaXN0b3J5IjpbLTEzMTQzMzMwNjUsLTU5ODYzMDcxMiwxOT
I2NjY1NTY3XX0=
-->
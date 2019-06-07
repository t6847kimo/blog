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

Facebook主要是利用[Graph API](https://developers.facebook.com/docs/graph-api?locale=zh_TW) 來Query User Data, 如官網描述

> 圖形 API 是應用程式讀取與寫入 Facebook 社交關係圖的主要方法。我們所有的 SDK 和產品皆會以某些方式與圖形 API 有所互動，而且其他 API 都是圖形 API 的擴充功能，所以瞭解圖形 API 的運作方式十分重要。

而Graph API主要的**Protocol**為**Http**, 所以基本上沒有平台或程式語言限制, 使用上非常便利. <br>
如果對細節可以參考Graph API的官網, 有很詳細的說明. 這邊我們Focus在怎麼使用就好~

大致上流程如下, 2&3主要用[圖形API測試工具](https://developers.facebook.com/tools/explorer/)來Demo, 不會真的架在Server上.
1. 註冊Facebook APP
2. 取得使用者授權後的Access Token
3. Query Data by Access Token

### 註冊一個Facebook的App
要從Graph API取得資料必須要經過User授權, 而授權的對象就是Facebook App, 所以我們必須先註冊一個Facebook App. <br>
申請方式不難, 任何人都可以進到[Facebook for developers](https://developers.facebook.com/apps/)申請. <br>
細節可以參考這篇[[教學]如何申請建立 Facebook APP ID 應用程式ID ？](https://sofree.cc/apply-facebook-app-id/)

### 取得Access Token

User授權給Facebook App的東西就是Access Token, 可以把他當成是使用者提供給這個App的Key<br>
拿到Key之後就可以用Grap API來Query User Data. <br>
進到[圖形API測試工具](https://developers.facebook.com/tools/explorer/)之後可以看到如下畫面

![Graph API 1](https://raw.githubusercontent.com/t6847kimo/blog/master/assets/img/Graph%20API%201.PNG)

接著點右邊"Get Access Token"按鈕來取得, 就會看到很熟悉的授權頁面

![Graph API 2](https://raw.githubusercontent.com/t6847kimo/blog/master/assets/img/Graph%20API%20%202.PNG)

### Query Data by Access Token

拿到Token之後, 就可以開始向FB Query Data了

如下圖最基本範例 , Query的對象為 `me`, 欄位為 `id,name`

![Graph API 3](https://raw.githubusercontent.com/t6847kimo/blog/master/assets/img/Graph%20API%20%203.PNG)

但我們要的內容為打卡資訊, 所以要在fields加上`post`欄位來取得貼文, 接著再從貼文中撈出有地點的資訊

詳細請參考[Graph API文件](https://developers.facebook.com/docs/graph-api/reference/v3.3/post)

### 取得更多授權

但是在新增欄位之後再Query, 會發現`post`部分是灰的, 這代表**App沒有權限取得貼文資訊**, 所以需要User再授權貼文資訊給App

![Graph API 4](https://raw.githubusercontent.com/t6847kimo/blog/master/assets/img/Graph%20API%20%204.PNG)

在右側選擇**新增權限: user_posts**, 再按**Get Access Token**, 即可獲得授權<br>
(麻煩的是FB因為隱私問題一直在限縮App的權限, 能用到什麼時候就看造化了...)

![Graph API 5](https://raw.githubusercontent.com/t6847kimo/blog/master/assets/img/Graph%20API%20%205.PNG)

再次按Query即可看到data, 不過會發現並沒有打卡的資訊, 因為我們沒有指定要貼文的哪些欄位, 所以預設是不會撈的

![Graph API 6](https://raw.githubusercontent.com/t6847kimo/blog/master/assets/img/Graph%20API%20%206.PNG)

### Query with place information in post

在左邊可以看到post是巢狀結構, 代表在post裡面我們可以繼續決定要Query哪些欄位.<br>
選擇 `place, message` 來取得貼文內容以及打卡資訊

 `me?fields=id,name,posts{place,message}`

麻煩的地方是這會連沒有打卡資訊的一起撈出來, 雖然可以再過濾掉就好

不過如果有人知道怎麼正確下Query的話請告訴我!!

![Graph API 7](https://raw.githubusercontent.com/t6847kimo/blog/master/assets/img/Graph%20API%20%207.PNG)

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
eyJoaXN0b3J5IjpbMTkyNjY2NTU2NywzMDM3NzI2N119
-->
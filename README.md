# VkAutoReply
Qt autoreply bot for vk.com

## getting vk application token
[Authorize](https://oauth.vk.com/authorize?client_id=5653619&display=page&redirect_uri=https://oauth.vk.com/blank.html&scope=messages,offline,&response_type=token&v=5.52) the app or [create your own](https://vk.com/dev/access_token)

## build
`cd build && qmake .. && make`

## run
`./vkautoreply -p patterns.txt -d 1000 -t (your app token there)`

#Autoreply bot behaviour
(TODO add loLang patterns description)

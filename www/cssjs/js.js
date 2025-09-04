/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   js.js                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yohanafi <yohanafi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 15:55:29 by yohanafi          #+#    #+#             */
/*   Updated: 2025/05/26 16:36:32 by yohanafi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


let image = [];
let currentIndex = 0;

function UpdateImage() {
    if (image.length > 0)
        document.getElementsByClassName("galleryImage").src = "../upload/" + image[currentIndex];
    else
        document.getElementsByClassName("galleryImage").src = "";
}

function nextImage() {
    if (image.length === 0)
        return;
    currentIndex = (currentIndex + 1) % ImageTrackList.length;
    UpdateImage();
}

function prevImage() {
    if (image.length === 0)
        retun;
    currentIndex = (currentIndex - 1 + image.length) % image.length;
    UpdateImage();
}

function deleteImage() {
    const imageToDelete = image[currentIndex];

    fetch("/" + encodeURIComponent("upload/" + imageToDelete), {
        method: "DELETE",
    })
        .then(response => {
            if (response.ok) {
                alert("image delete :D : " + imageToDelete);
                image.splice(currentIndex, 1);
                if (image.length === 0)
                    document.getElementsByClassName("galleryImage").src = "";
                else {
                    currentIndex = currentIndex % image.length;
                    UpdateImage();
                }
            } else {
                alert("Erreur lors de la suppression.")
            }
        });
}

function
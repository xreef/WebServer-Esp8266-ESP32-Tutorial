var sliderInitialization = function(  sliderElement,
                                sliderImageElements,
                                holderElement,
                                animateElements,
                              elements) {
    if (navigator.msMaxTouchPoints) {


        sliderElement.classList.add('ms-touch');

        sliderElement.addEventListener('scroll', function () {
            // var sliderImageElements = document.querySelectorAll(".slide-wrapper");
            for (var i1 = 0; i1 < sliderElements.length; i1++) {
                var sliderImageElement = sliderImageElements[i1];
                sliderImageElement.style.transform = 'translate3d(-' + (100 - sliderElement.scrollLeft() / (elements * 2)) + 'px,0,0)';
            }
            // $('.slide-image').css('transform','translate3d(-' + (100-$(this).scrollLeft()/6) + 'px,0,0)');
        });


    } else {

        // var holderElement = document.querySelector(".holder"); //$(".holder");
        // var sliderImageElement = document.querySelectorAll(".slide-wrapper");

        var slider = {

            el: {
                slider: sliderElement,
                holder: holderElement,
                imgSlides: sliderImageElements
            },

            slideWidth: sliderElement.offsetWidth,
            touchstartx: undefined,
            touchmovex: undefined,
            movex: undefined,
            index: 0,
            longTouch: undefined,

            init: function () {
                this.bindUIEvents();
            },

            bindUIEvents: function () {

                this.el.holder.addEventListener("touchstart", function (event) {
                    slider.start(event);
                });

                this.el.holder.addEventListener("touchmove", function (event) {
                    slider.move(event);
                });

                this.el.holder.addEventListener("touchend", function (event) {
                    slider.end(event);
                });

                var holder = this.el.holder;

                var moveFunct = function (event) {
                    slider.move(event);
                };
                holder.addEventListener("mousedown", function (event) {
                    event.preventDefault();
                    slider.start(event);
                    holder.addEventListener("mousemove", moveFunct);
                    var mouseUpFunct = function (event) {
                        holder.removeEventListener("mousemove", moveFunct);
                        slider.end(event);

                        holder.removeEventListener("mouseup", mouseUpFunct);
                        holder.removeEventListener("mouseout", mouseUpFunct);
                    };
                    holder.addEventListener("mouseup", mouseUpFunct);
                    holder.addEventListener("mouseout", mouseUpFunct);
                });


            },

            start: function (event) {
                // Test for flick.
                this.longTouch = false;
                setTimeout(function () {
                    window.slider.longTouch = true;
                }, 250);

                // Get the original touch position.
                this.touchstartx = (event.touches && event.touches[0].pageX) || event.pageX;

                // var animateElements = document.querySelectorAll('.animate');
                // The movement gets all janky if there's a transition on the elements.
                for (var iAnimateElements = 0; iAnimateElements < animateElements.length; iAnimateElements++) {
                    var animateElement = iAnimateElements[iAnimateElements];
                    if (animateElement) animateElement.classList.remove('animate');
                }
            },

            move: function (event) {
                // Continuously return touch position.
                this.touchmovex = (event.touches && event.touches[0].pageX) || event.pageX;
                // this.touchmovex =  event.touches[0].pageX || event.pageX;
                // Calculate distance to translate holder.
                this.movex = this.index * this.slideWidth + (this.touchstartx - this.touchmovex);
                // Defines the speed the images should move at.
                var panx = 100 - this.movex / (elements * 2);
                if (this.movex < (200 * elements)) { // Makes the holder stop moving when there is no more content.
                    this.el.holder.style.transform = 'translate3d(-' + this.movex + 'px,0,0)';
                }
                // if (panx < 100) { // Corrects an edge-case problem where the background image moves without the container moving.
                //     for (var iimgSlides = 0; iimgSlides < this.el.imgSlides.length; iimgSlides++) {
                //         var imgSlide = this.el.imgSlides[iimgSlides];
                //         imgSlide.style.transform = 'translate3d(-' + panx + 'px,0,0)';
                //     }
                // }
            },

            end: function (event) {
                // Calculate the distance swiped.
                var absMove = Math.abs(this.index * this.slideWidth - this.movex);
                // Calculate the index. All other calculations are based on the index.
                if (absMove > this.slideWidth / 2 || this.longTouch === false) {
                    if (this.movex > this.index * this.slideWidth && this.index < (elements - 1)) {
                        this.index++;
                    } else if (this.movex < this.index * this.slideWidth && this.index > 0) {
                        this.index--;
                    }
                }
                // Move and animate the elements.
                this.el.holder.classList.add('animate');
                this.el.holder.style.transform = 'translate3d(-' + this.index * this.slideWidth + 'px,0,0)';
                // for (var iimgSlides = 0; iimgSlides < this.el.imgSlides.length; iimgSlides++) {
                //     var imgSlide = this.el.imgSlides[iimgSlides];
                //     imgSlide.classList.add('animate');
                //     imgSlide.style.transform = 'translate3d(-' + 100 - this.index * 50 + 'px,0,0)';
                // }

            }

        };

        slider.init();
    }
}

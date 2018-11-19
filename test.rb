
def cartesianProduct(*args)
    result = [[]]
    while [] != args
        t, result = result, []
        b, *args = args
        t.each do |a|
            b.each do |n|
                result << a + [n]
            end
        end
    end
    result
end
def powersOfFour(max, min=1)
    max <= min ? [max] : powersOfFour(max/4, min) << max
end


word_right_before_time_units = "Duration:"
n = 10000
w = 1024
b = 256

# all_combos = cartesianProduct(powersOfFour(w,4), powersOfFour(b,4))
all_combos = [
    [1, 1,],
    [4, 4,],
    [4, 16,],
    [4, 64,],
    [4, 256,],
    [16, 4,],
    [16, 16,],
    [16, 64,],
    [16, 256,],
    [64, 4,],
    [64, 16,],
    [64, 64,],
    [64, 256,],
    [256, 4,],
    [256, 16,],
    [256, 64,],
    [256, 256,],
    [500, 4,],
    [500, 16,],
    [500, 64,],
    [500, 256,],
]
all_combos.reverse!
p all_combos
for w, b in all_combos
    print "#{w}, #{b}, "
    output =  `./client -n #{n} -w #{w} -b #{b}`
    just_the_miliseconds = output.gsub(/[\s\S]*?#{word_right_before_time_units}.*?(\d+)[\s\S\n]*/, "\\1")
    puts just_the_miliseconds
end

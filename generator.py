import sys
import random
import string

TEST_COUNT = 3
MAX_LINES = 8

def toFixed(numObj, digits=0):
    return f"{numObj:.{digits}f}"


def main():
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <test directory>")
        sys.exit(1)

    test_dir = sys.argv[1]

    lines = [0]
    lines.extend([ 10 ** i for i in range(MAX_LINES) ])
    


    for enum, test_count in enumerate(range(1, TEST_COUNT+1)):
        test = []
        answer = []
        test_name = "{}/{:02d}".format(test_dir, test_count)
        print(f">> Test {test_count} start generating ...")

        line_count = lines[enum]
        for _ in range(line_count):
            float1 = random.uniform( -100.2, 150.9 )
            float2 = random.uniform( -100.2, 150.9 )
            float3 = random.uniform( -100.2, 150.9 )
            test.append((float1, float2, float3))

        print(f">> Test {test_count} start writing ...")
        with open(f'{test_name}.t', 'w') as ftest:
            for float1, float2, float3 in test:
                ftest.write('{0} {1} {2}\n'.format( toFixed(round(float1, 4), 4), toFixed(round(float2, 4), 4), toFixed(round(float3, 4), 4)))
        with open(f'{test_name}.src', 'w') as srctest:
            srctest.write(f'{test_name}.t')

        print(f">> Test {test_count} start computing ...")
        for float1, float2, float3 in test:
            answer.append( float1 / float2 )
            answer.append( float1 / float3 )
        

        print(f">> Computed test {test_count} start writing ...")
        with open(f'{test_name}.a', 'w') as ftest:
            for res in answer:
                ftest.write('Received from child {} \n'.format( toFixed(round(res, 4), 4) ))
        print("-----------------------------------------------")
main()

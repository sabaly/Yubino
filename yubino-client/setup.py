from setuptools import setup, find_packages

setup(
    name='yubino',
    version='0.0.1',
    author='Vincent Ruello',
    author_email='vincent.ruello@cea.fr',
    description="Yubino client",
    packages=find_packages(),
    entry_points={
        'console_scripts': [
            'yubino = yubino.main:main'
        ]
    },
    install_requires = [
        'pyserial',
        'requests',
        'ecdsa',
    ],
    test_suite='tests',
)
